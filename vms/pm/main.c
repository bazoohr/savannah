#include <cdef.h>
#include <types.h>
#include <const.h>
#include <asmfunc.h>
#include <memory.h>
#include <cpu.h>
#include <string.h>
#include <ipc.h>
#include <pm.h>
#include <fs.h>
#include <pm_args.h>
#include <pmap.h>
#include <elf.h>
#include <panic.h>
#include <macro.h>
#include <debug.h>
#include <misc.h>
#include <vuos/vuos.h>
#include <channel.h>
#include <interrupt.h>
/* ================================================= */
#define ALWAYS_BUSY (NUMBER_SERVERS + NUMBER_USER_VMS)
/* ================================================= */
#define BUSY true
#define FREE false
/* ================================================= */
static char arg_vector[MAX_ARGV][MAX_ARGV_LEN] __aligned (0x10);  /* store exec arguments */
static virt_addr_t argv_ptr[MAX_ARGV] __aligned (0x10); /* virtual addresses of exec arguments*/
/* ================================================= */
static bool has_1GB_page;
/* ================================================= */
static __inline size_t
pages (size_t sz, size_t pgsz)
{
  return (sz / pgsz + ((sz % pgsz > 0) ? 1 : 0));
}
/* ================================================= */
static __inline struct cpu_info *
get_cpu_info (const cpuid_t cpuid)
{
  phys_addr_t cpuinfo_base_paddr;

  if (cpuid > MAX_CPUS) {
    return NULL;
  }

  /*
   * TODO:
   *     Create a macro which hold the maximum size of cpu_info structure, and
   *     use it here (AND EVERYWHERE)  instead of _4KB_
   */
  cpuinfo_base_paddr = ((phys_addr_t)cpuinfo - (/*PM's cpuid */cpuinfo->cpuid * _4KB_));

  /* TODO:
   *     create a function and intitialize if the cpu_info structure is
   *     greater than _4KB_
   */
  /* TODO:
   *     One test to be done, is to allocate more than one page for one cpuuinfo
   *     structure, and solve the mess created afterward.
   */
  return (struct cpu_info *)(cpuinfo_base_paddr + cpuid * _4KB_);
}
/* ================================================= */
static __inline struct cpu_info *
find_free_vm (void)
{
  struct cpu_info *cpu;
  aint i;
  for (i = ALWAYS_BUSY; i < cpuinfo->ncpus; i++) {
    cpu = get_cpu_info (i);
    if (!cpu) {
      panic ("pm (find_free_vm): could not get cpu information!");
    }
    if (!cpu->vmm_info.vmm_has_vm) {
      return cpu;
    }
  }
  return NULL;
}
/* ================================================= */
static __inline void
pm_init (void)
{
  struct pm_args *pm_arguments __aligned (0x10);

  pm_arguments = (struct pm_args*)cpuinfo->vm_args;
  has_1GB_page = pm_arguments->has_1GB_page;
  memory_init (pm_arguments->last_used_addr, pm_arguments->memory_size);
}
/* ================================================= */
static __inline void
ept_pmap (struct cpu_info * const child_cpu_info)
{
  map_memory (&child_cpu_info->vm_info.vm_page_tables,
      0, (virt_addr_t)((virt_addr_t)_1GB_ * 3),
      0,
      has_1GB_page ? _1GB_ : _2MB_,
      PAGE_PRESENT | PAGE_RW | PAGE_PWT, MAP_NEW);
      /* Map Last 1GB as uncachable memory for MMIO */
  map_memory (&child_cpu_info->vm_info.vm_page_tables,
      (virt_addr_t)((virt_addr_t)_1GB_ * 3), (virt_addr_t)((virt_addr_t)_1GB_ * 4),
      (virt_addr_t)((virt_addr_t)_1GB_ * 3),
      has_1GB_page ? _1GB_ : _2MB_,
      PAGE_PRESENT | PAGE_RW | PAGE_PCD, MAP_UPDATE);
  ept_map_memory (&child_cpu_info->vm_info.vm_ept,
      0xB8000, 0xB9000,  /* Just for debugging purposes */
      0xB8000,
      USER_VMS_PAGE_SIZE,
      EPT_MTYPE_WT,
      EPT_PAGE_READ | EPT_PAGE_WRITE, MAP_NEW);
  ept_map_memory (&child_cpu_info->vm_info.vm_ept,
      0xFEC00000, ((phys_addr_t)0x100000000),
      0xFEC00000,
      USER_VMS_PAGE_SIZE,
      EPT_MTYPE_UC,
      EPT_PAGE_READ | EPT_PAGE_WRITE, MAP_UPDATE);  /* XXX: Why do we need write access here? */

  ept_map_page_tables (&child_cpu_info->vm_info.vm_ept, child_cpu_info->vm_info.vm_page_tables, EPT_PAGE_READ | EPT_PAGE_WRITE);

  ept_map_memory (&child_cpu_info->vm_info.vm_ept,
      (phys_addr_t)child_cpu_info, (phys_addr_t)child_cpu_info + _4KB_,
      (phys_addr_t)child_cpu_info,
      USER_VMS_PAGE_SIZE,
      EPT_MTYPE_WT,
      EPT_PAGE_READ, MAP_UPDATE);
  ept_map_memory (&child_cpu_info->vm_info.vm_ept,
      (phys_addr_t)child_cpu_info->msg_input, (phys_addr_t)child_cpu_info->msg_input + _4KB_,
      (phys_addr_t)child_cpu_info->msg_input,
      USER_VMS_PAGE_SIZE,
      EPT_MTYPE_WT,
      EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);
  ept_map_memory (&child_cpu_info->vm_info.vm_ept,
      (phys_addr_t)child_cpu_info->msg_output, (phys_addr_t)child_cpu_info->msg_output + _4KB_,
      (phys_addr_t)child_cpu_info->msg_output,
      USER_VMS_PAGE_SIZE,
      EPT_MTYPE_WT,
      EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);
  ept_map_memory (&child_cpu_info->vm_info.vm_ept,
      (phys_addr_t)child_cpu_info->msg_ready, (phys_addr_t)child_cpu_info->msg_ready + _4KB_,
      (phys_addr_t)child_cpu_info->msg_ready,
      USER_VMS_PAGE_SIZE,
      EPT_MTYPE_WT,
      EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);

  if (child_cpu_info->vm_info.vm_code_size > 0) {
    ept_map_memory (&child_cpu_info->vm_info.vm_ept,
        child_cpu_info->vm_info.vm_code_vaddr, child_cpu_info->vm_info.vm_code_vaddr + child_cpu_info->vm_info.vm_code_size,
        child_cpu_info->vm_info.vm_code_paddr,
        USER_VMS_PAGE_SIZE,
        EPT_MTYPE_WT,
        EPT_PAGE_READ | EPT_PAGE_EXEC, MAP_UPDATE);
  }
  if (child_cpu_info->vm_info.vm_data_size > 0) {
    ept_map_memory (&child_cpu_info->vm_info.vm_ept,
        child_cpu_info->vm_info.vm_data_vaddr, child_cpu_info->vm_info.vm_data_vaddr + child_cpu_info->vm_info.vm_data_size,
        child_cpu_info->vm_info.vm_data_paddr,
        USER_VMS_PAGE_SIZE,
        EPT_MTYPE_WT,
        EPT_PAGE_READ | EPT_PAGE_EXEC, MAP_UPDATE);
  }
  if (child_cpu_info->vm_info.vm_rodata_size > 0) {
    ept_map_memory (&child_cpu_info->vm_info.vm_ept,
        child_cpu_info->vm_info.vm_rodata_vaddr, child_cpu_info->vm_info.vm_rodata_vaddr + child_cpu_info->vm_info.vm_rodata_size,
        child_cpu_info->vm_info.vm_rodata_paddr,
        USER_VMS_PAGE_SIZE,
        EPT_MTYPE_WT,
        EPT_PAGE_READ, MAP_UPDATE);
  }
  if (child_cpu_info->vm_info.vm_bss_size > 0) {
    ept_map_memory (&child_cpu_info->vm_info.vm_ept,
        child_cpu_info->vm_info.vm_bss_vaddr, child_cpu_info->vm_info.vm_bss_vaddr + child_cpu_info->vm_info.vm_bss_size,
        child_cpu_info->vm_info.vm_bss_paddr,
        USER_VMS_PAGE_SIZE,
        EPT_MTYPE_WT,
        EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);
  }
  if (child_cpu_info->vm_info.vm_stack_size > 0) {
    ept_map_memory (&child_cpu_info->vm_info.vm_ept,
        child_cpu_info->vm_info.vm_stack_vaddr, child_cpu_info->vm_info.vm_stack_vaddr + child_cpu_info->vm_info.vm_stack_size,
        child_cpu_info->vm_info.vm_stack_paddr,
        USER_VMS_PAGE_SIZE,
        EPT_MTYPE_WT,
        EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);
  }
  if (child_cpu_info->vm_info.vm_fds > 0) {
    ept_map_memory (&child_cpu_info->vm_info.vm_ept,
        (phys_addr_t)child_cpu_info->vm_info.vm_fds, (phys_addr_t)child_cpu_info->vm_info.vm_fds + _4KB_,
        (phys_addr_t)child_cpu_info->vm_info.vm_fds,
        USER_VMS_PAGE_SIZE,
        EPT_MTYPE_WT,
        EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);
    int i;
    for (i = 0 ; i < MAX_FD ; i++) {
      if (child_cpu_info->vm_info.vm_fds[i].offset != 0) {
        ept_map_memory (&child_cpu_info->vm_info.vm_ept,
            (phys_addr_t)child_cpu_info->vm_info.vm_fds[i].offset, (phys_addr_t)child_cpu_info->vm_info.vm_fds[i].offset + _4KB_,
            (phys_addr_t)child_cpu_info->vm_info.vm_fds[i].offset,
            USER_VMS_PAGE_SIZE,
            EPT_MTYPE_WT,
            EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);
      }
    }
  }
  if (child_cpu_info->vm_info.vm_channels > 0) {
    ept_map_memory (&child_cpu_info->vm_info.vm_ept,
        (phys_addr_t)child_cpu_info->vm_info.vm_channels, (phys_addr_t)child_cpu_info->vm_info.vm_channels + _4KB_,
        (phys_addr_t)child_cpu_info->vm_info.vm_channels,
        USER_VMS_PAGE_SIZE,
        EPT_MTYPE_WT,
        EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);
  }
}
/* ================================================= */
static void
remove_channel_list (phys_addr_t channel)
{
  if (channel == 0) {
    panic ("Removing channel 0!\n");
  }

  struct cpu_info *info;
  int i, j, k;
  int found = -1;

  for (i = 0 ; i < cpuinfo->ncpus ; i++) {
    info = get_cpu_info(i);

    if (info->vm_info.vm_channels == NULL) {
      continue;
    }

    for (j = 0 ; j < MAX_CHANNELS ; j++) {
      if (info->vm_info.vm_channels[j] == channel) {
        found = j;
        for (k = j ; k < MAX_CHANNELS - 1 ; k++) {
          if (info->vm_info.vm_channels[k] == 0) {
            break;
          }

          info->vm_info.vm_channels[k] = info->vm_info.vm_channels[k + 1];
        }
	// Just to be sure that there is always a 0 at the end.
	info->vm_info.vm_channels[k + 1] = 0;
      }
    }
  }

  if (found == -1) {
    panic ("Channel %x not found!\n", channel);
  }
}
/* ================================================= */
static void
add_channel_list (struct cpu_info *info, phys_addr_t channel)
{
  if (info->vm_info.vm_channels == NULL) {
    panic ("Cpu does not have any channel!\n");
  }

  if (channel == 0) {
    panic ("Channel is 0!\n");
  }

  int i;

  for (i = 0 ; i < MAX_CHANNELS ; i++) {
    if (info->vm_info.vm_channels[i] == 0) {
      info->vm_info.vm_channels[i] = channel;
      break;
    }
  }
}
/* ================================================= */
static phys_addr_t
local_channel (const struct channel_ipc * const req)
{
  struct cpu_info *cpu1;
  struct cpu_info *cpu2;
  phys_addr_t channel;

  cpu1 = get_cpu_info (req->end1);
  cpu2 = get_cpu_info (req->end2);

  if (unlikely (cpu1->vmm_info.vmm_has_vm == false)) {
    panic ("PM: creating channel failed! VM1 does not exist! VM1 = %d & VM2 = %d!\n", cpu1->cpuid, cpu2->cpuid);
  }

  if (unlikely (cpu2->vmm_info.vmm_has_vm == false)) {
    panic ("PM: creating channel failed! VM2 does not exist! VM1 = %d & VM2 = %d!\n", cpu1->cpuid, cpu2->cpuid);
  }

  if (unlikely (!is_driver(cpu2->cpuid))) {
    panic ("PM: The second end of the channel needs to be always a driver");
  }

  channel = (phys_addr_t)alloc_clean_mem_pages (pages (CHANNEL_SIZE, USER_VMS_PAGE_SIZE));
  if (unlikely (channel == 0)) {
    panic ("PM: Failed to alloced memory!");
  }

  ept_map_memory (&cpu1->vm_info.vm_ept,
      channel,  channel + CHANNEL_SIZE,
      channel,
      USER_VMS_PAGE_SIZE,
      EPT_MTYPE_WT,
      EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);

  ept_map_memory (&cpu2->vm_info.vm_ept,
      channel,  channel + CHANNEL_SIZE,
      channel,
      USER_VMS_PAGE_SIZE,
      EPT_MTYPE_WT,
      EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);

  add_channel_list(cpu2, channel);

  return channel;
}
/* ================================================= */
static void
local_channel_close (const virt_addr_t channel)
{
  remove_channel_list(channel);
  free_mem_pages (channel);
}
/* ================================================= */
static pid_t
local_fork (const struct cpu_info * const info, const struct fork_ipc * const fork_args)
{
  acpuid_t parent_id;
  struct cpu_info *child_cpu_info;
  struct cpu_info *parent_cpu_info;
  struct cpu_info **child_cpu_info_ptr;
  asize_t offset;

  struct channel_ipc channelipc;
  int i;
  phys_addr_t channel;

  if (info == NULL) {
    return -1;
  }

  parent_id = info->cpuid;

  child_cpu_info = find_free_vm ();
  if (!child_cpu_info) {
    return -1;
  }
  child_cpu_info->vmm_info.vmm_has_vm = true;

  parent_cpu_info = get_cpu_info (parent_id);

  child_cpu_info->vm_info.vm_parent = parent_id;
  child_cpu_info->vm_info.vm_state = RUNNING;
  /*
   * TODO:
   *     If any of memory allocations fails, we have to free all other allocated
   *     memory blocks, and set
   *     child_cpu_info->vmm_info.vmm_has_vm = false;
   *
   *     Currently we assume that memory allocation does not fail! Such a STRONG assumption!!!
   */
  child_cpu_info->vm_info.vm_start_vaddr = parent_cpu_info->vm_info.vm_start_vaddr;
  child_cpu_info->vm_info.vm_start_paddr = parent_cpu_info->vm_info.vm_start_paddr;

  child_cpu_info->vm_info.vm_end_vaddr = parent_cpu_info->vm_info.vm_end_vaddr;
  child_cpu_info->vm_info.vm_end_paddr = parent_cpu_info->vm_info.vm_end_paddr;
  /* code */
  child_cpu_info->vm_info.vm_code_vaddr = parent_cpu_info->vm_info.vm_code_vaddr;
  child_cpu_info->vm_info.vm_code_size  = parent_cpu_info->vm_info.vm_code_size;
  if (likely (reallocable (parent_cpu_info->vm_info.vm_code_paddr))) {
    child_cpu_info->vm_info.vm_code_paddr = (phys_addr_t)realloc_mem_block (parent_cpu_info->vm_info.vm_code_paddr);
  } else {
    child_cpu_info->vm_info.vm_code_paddr = (phys_addr_t)alloc_mem_pages (pages (parent_cpu_info->vm_info.vm_code_size, USER_VMS_PAGE_SIZE));
    memcpy ((void*)child_cpu_info->vm_info.vm_code_paddr, (void*)parent_cpu_info->vm_info.vm_code_paddr, parent_cpu_info->vm_info.vm_code_size);
  }
  /* data */
  child_cpu_info->vm_info.vm_data_paddr = (phys_addr_t)alloc_mem_pages (pages (parent_cpu_info->vm_info.vm_data_size, USER_VMS_PAGE_SIZE));
  child_cpu_info->vm_info.vm_data_vaddr = parent_cpu_info->vm_info.vm_data_vaddr;
  child_cpu_info->vm_info.vm_data_size  = parent_cpu_info->vm_info.vm_data_size;
  memcpy ((void*)child_cpu_info->vm_info.vm_data_paddr, (void*)parent_cpu_info->vm_info.vm_data_paddr, parent_cpu_info->vm_info.vm_data_size);
  /* Rodata */
  child_cpu_info->vm_info.vm_rodata_vaddr = parent_cpu_info->vm_info.vm_rodata_vaddr;
  child_cpu_info->vm_info.vm_rodata_size  = parent_cpu_info->vm_info.vm_rodata_size;
  if (likely (reallocable (parent_cpu_info->vm_info.vm_rodata_paddr))) {
    child_cpu_info->vm_info.vm_rodata_paddr = (phys_addr_t)alloc_mem_pages (pages (parent_cpu_info->vm_info.vm_rodata_size, USER_VMS_PAGE_SIZE));
  } else {
    child_cpu_info->vm_info.vm_rodata_paddr = (phys_addr_t)alloc_mem_pages (pages (parent_cpu_info->vm_info.vm_rodata_size, USER_VMS_PAGE_SIZE));
    memcpy ((void*)child_cpu_info->vm_info.vm_rodata_paddr, (void*)parent_cpu_info->vm_info.vm_rodata_paddr, parent_cpu_info->vm_info.vm_rodata_size);
  }
  /* bss */
  /* We don't allocate clean pages here for bss, because anyways we're going to copy parent's data into that. */
  child_cpu_info->vm_info.vm_bss_paddr = (phys_addr_t)alloc_mem_pages (pages (parent_cpu_info->vm_info.vm_bss_size, USER_VMS_PAGE_SIZE));
  child_cpu_info->vm_info.vm_bss_vaddr = parent_cpu_info->vm_info.vm_bss_vaddr;
  child_cpu_info->vm_info.vm_bss_size  = parent_cpu_info->vm_info.vm_bss_size;
  memcpy ((void*)child_cpu_info->vm_info.vm_bss_paddr, (void*)parent_cpu_info->vm_info.vm_bss_paddr, parent_cpu_info->vm_info.vm_bss_size);
  /* stack */
  child_cpu_info->vm_info.vm_stack_paddr = (phys_addr_t)alloc_mem_pages (pages (parent_cpu_info->vm_info.vm_stack_size, USER_VMS_PAGE_SIZE));
  child_cpu_info->vm_info.vm_stack_vaddr = parent_cpu_info->vm_info.vm_stack_vaddr;
  child_cpu_info->vm_info.vm_stack_size  = parent_cpu_info->vm_info.vm_stack_size;
  struct regs *parent_regs = (struct regs *)fork_args->register_array_paddr;
  void *child_rsp_paddr  = (void*)virt2phys (child_cpu_info, parent_regs->rsp);
  void *parent_rsp_paddr = (void*)virt2phys (parent_cpu_info, parent_regs->rsp);
  if (unlikely (child_rsp_paddr == NULL)) {
    panic ("fork fatal: virt2phys failed for child stack");
  }
  if (unlikely (parent_rsp_paddr == NULL)) {
    panic ("fork fatal: virt2phys failed for parent stack");
  }
  memcpy (child_rsp_paddr,
          parent_rsp_paddr,
          parent_cpu_info->vm_info.vm_stack_vaddr + parent_cpu_info->vm_info.vm_stack_size -
          parent_regs->rsp);
  /* Allocate memory for FDs if the process is not a driver */
  if (!is_driver(child_cpu_info->cpuid)) {
    child_cpu_info->vm_info.vm_fds = (struct file_descriptor*)alloc_clean_mem_pages (pages (MAX_FD * sizeof(struct file_descriptor), USER_VMS_PAGE_SIZE));
  }

  /* Allocate memory for channels for all the drivers */
  if (is_driver(child_cpu_info->cpuid)) {
    child_cpu_info->vm_info.vm_channels = (virt_addr_t*)alloc_clean_mem_pages (pages (MAX_CHANNELS * sizeof(virt_addr_t), USER_VMS_PAGE_SIZE));
  }

  /*
   * VM's page tables are already build in boot/stage2
   * so we only need to map ept
   */
  ept_pmap (child_cpu_info);

  /* Copy the FDs from the parent to the child */
  if (child_cpu_info->vm_info.vm_fds) {
    /* If parent has FDs */
    if (parent_cpu_info->vm_info.vm_fds != NULL) {
      /* Copy all the fds from the parent to the child */
      memcpy(child_cpu_info->vm_info.vm_fds, parent_cpu_info->vm_info.vm_fds, MAX_FD * sizeof(struct file_descriptor));

      /* For all the CHAR fds create a new channel for the child */
      for (i = 0 ; i < MAX_FD ; i++) {
        if (child_cpu_info->vm_info.vm_fds[i].type == TYPE_CHAR) {
          channelipc.end1 = child_cpu_info->cpuid;
          channelipc.end2 = child_cpu_info->vm_info.vm_fds[i].dst;
          channel = local_channel (&channelipc);
          child_cpu_info->vm_info.vm_fds[i].offset = channel;
        }
      }
    }
  }

  memcpy ((void *)&child_cpu_info->vm_info.vm_regs, (void *)fork_args->register_array_paddr, sizeof (struct regs));

  /*
   * TODO:
   * variable  cpuinfo might not be used in bss section. So here, code should be defensive, and check to find out
   * exactly in which section cpuinfo is stored! At the present time, I just write the code for section BSS to get
   * the job done, but this is NOT the proper way to do this.
   */
  offset = virt2phys (parent_cpu_info, fork_args->cpuinfo_vaddr) - parent_cpu_info->vm_info.vm_bss_paddr;
  child_cpu_info_ptr = (struct cpu_info **)((phys_addr_t)child_cpu_info->vm_info.vm_bss_paddr + offset);
  *child_cpu_info_ptr = child_cpu_info;

  child_cpu_info->vm_info.vm_regs.rax = 0;

#if 0
  DEBUG ("code p = %x v = %x\ndata p = %x v = %x\nrodata p = %x v = %x\nbss p = %x v = %x\nstack p = %x v = %x\n pgtb = %x ept = %x\n", 0xF, child_cpu_info->vm_info.vm_code_paddr, child_cpu_info->vm_info.vm_code_vaddr,
      child_cpu_info->vm_info.vm_data_paddr, child_cpu_info->vm_info.vm_data_vaddr,
      child_cpu_info->vm_info.vm_rodata_paddr, child_cpu_info->vm_info.vm_rodata_vaddr,
      child_cpu_info->vm_info.vm_bss_paddr, child_cpu_info->vm_info.vm_bss_vaddr,
      child_cpu_info->vm_info.vm_stack_paddr, child_cpu_info->vm_info.vm_stack_vaddr,
      child_cpu_info->vm_info.vm_page_tables, child_cpu_info->vm_info.vm_ept);
  DEBUG ("========================\n", 0xF);
  __asm__ __volatile__ ("cli;hlt\n\t");
#endif
  return child_cpu_info->cpuid;
}
/* ================================================= */
static void
load_elf (struct cpu_info * const curr_cpu_info, const phys_addr_t elf)
{
  asize_t vm_size;     /* VMM's Code + data + rodata + bss + stack */
  aint ph_num;          /* VMM's number of program headers */
  aint i;
  Elf64_Ehdr *elf_hdr __aligned (0x10);  /* Start address of executable */
  Elf64_Phdr *s __aligned (0x10);

  phys_addr_t old_pcode;
  phys_addr_t old_pdata;
  phys_addr_t old_prodata;
  phys_addr_t old_pbss;
  phys_addr_t old_pstack;
  phys_addr_t old_pgtb;
  phys_addr_t old_ept;
  /*
   * stage2 ELF header contains 4 sections. (look at stage2/link64.ld).
   * These sections are respectively
   *   1. text
   *   2. data
   *   3. rodata
   *   4. bss
   * If we first check to make sure we have exactly four sections in the
   * ELF file loaded by grub.
   * TODO:
   * Its also best if we check to make sure that each of these sections
   * contains exactly the content we expect. Checking only the number of
   * program headers is SO trivial and unsafe check.
   */
  /* ========================================= */
  /* Save information of current VM so that
   * we can free the memory occupied by it after
   * creating the new VM.
   * ========================================= */
  old_pcode = curr_cpu_info->vm_info.vm_code_paddr;
  old_pdata = curr_cpu_info->vm_info.vm_data_paddr;
  old_prodata = curr_cpu_info->vm_info.vm_rodata_paddr;
  old_pbss = curr_cpu_info->vm_info.vm_bss_paddr;
  old_pstack = curr_cpu_info->vm_info.vm_stack_paddr;
  old_pgtb = curr_cpu_info->vm_info.vm_page_tables;
  old_ept  = curr_cpu_info->vm_info.vm_ept;
  /* ========================================= */
  elf_hdr = (Elf64_Ehdr*)elf;  /* Start address of executable */
  /* ========================================= */

  ph_num = elf_hdr->e_phnum;    /* Number of program headers in ELF executable */
  if (ph_num != 4) {
    DEBUG ("ELF executable contains %d sections!\n", 0x4, ph_num);
    panic ("Unexpected VM Header!\n");
  }

  s = (Elf64_Phdr*)((uint8_t*)elf + elf_hdr->e_phoff);
  curr_cpu_info->vm_info.vm_start_vaddr = (virt_addr_t)s->p_vaddr;
  /* Why Upper bound is ph_num - 1?
   * Because in the VM ELF executable, last section is .bss which does not have any byes.
   * So we don't need to copy any bytes from this section into memory. But we do need to allocate
   * enough memory for this section when we load the executable, and we
   * also need to zero out allocated memory. That's why we put .bss section at the
   * end of executable.
   */
  for (i = 0; i < ph_num - 1; i++) {
    aphys_addr_t section_src_paddr = 0;
    aphys_addr_t section_dst_paddr = 0;
    asize_t section_size;

    section_src_paddr = elf + s->p_offset;  /* Address of section in executable */
    section_size = (size_t)s->p_memsz;               /* Section size */
    if (section_size > 0) {
      /* Address of section when loaded in ram */
      section_dst_paddr = (phys_addr_t)alloc_mem_pages (pages (section_size, USER_VMS_PAGE_SIZE));
    }

    switch (i) {
      case 0:  /* Code */
        if (section_size == 0) {
          panic ("VM without code section!");
        }
        curr_cpu_info->vm_info.vm_start_paddr = section_dst_paddr;
        curr_cpu_info->vm_info.vm_code_vaddr = s->p_vaddr;
        curr_cpu_info->vm_info.vm_code_paddr = section_dst_paddr;
        curr_cpu_info->vm_info.vm_code_size = section_size;
        curr_cpu_info->vm_info.vm_regs.rip = curr_cpu_info->vm_info.vm_start_vaddr;
        break;
      case 1:  /* Data */
        curr_cpu_info->vm_info.vm_data_vaddr = s->p_vaddr;
        curr_cpu_info->vm_info.vm_data_paddr = section_size > 0 ? section_dst_paddr : 0;
        curr_cpu_info->vm_info.vm_data_size = section_size;
        break;
      case 2:  /* Rodata */
        curr_cpu_info->vm_info.vm_rodata_vaddr = s->p_vaddr;
        curr_cpu_info->vm_info.vm_rodata_paddr = section_size > 0 ? section_dst_paddr : 0;
        curr_cpu_info->vm_info.vm_rodata_size = section_size;
        break;
      default:
        panic ("Unexpected VM ELF Header!");
    }

    if (section_size > 0) {
      memcpy ((void*)section_dst_paddr, (void*)section_src_paddr, section_size);
    }
    s++;    /* Go to next section */
  }

  /* Last section is bss. We zero out this section */
  curr_cpu_info->vm_info.vm_bss_vaddr = s->p_vaddr;

  if (s->p_memsz > 0) {
    curr_cpu_info->vm_info.vm_bss_paddr = (phys_addr_t)alloc_mem_pages (pages (_2MB_, USER_VMS_PAGE_SIZE));
    curr_cpu_info->vm_info.vm_bss_size = s->p_memsz;
    memset ((void*)curr_cpu_info->vm_info.vm_bss_paddr, 0, s->p_memsz);
  }
  /*
   * Here we want to reserve some space for the stack. Stack should be
   * 16 byte aligned!
   */
  curr_cpu_info->vm_info.vm_stack_vaddr = ALIGN ((virt_addr_t)s->p_vaddr + s->p_memsz, USER_VMS_PAGE_SIZE);
  curr_cpu_info->vm_info.vm_stack_size = _2MB_;
  curr_cpu_info->vm_info.vm_stack_paddr = (phys_addr_t)alloc_mem_pages (pages (_2MB_, USER_VMS_PAGE_SIZE));
  curr_cpu_info->vm_info.vm_regs.rsp = curr_cpu_info->vm_info.vm_stack_vaddr + curr_cpu_info->vm_info.vm_stack_size;

  curr_cpu_info->vm_info.vm_regs.rflags = 0x2; /* No Flags Set!!! */
  vm_size = (curr_cpu_info->vm_info.vm_stack_paddr + curr_cpu_info->vm_info.vm_stack_size) - curr_cpu_info->vm_info.vm_start_paddr;

  curr_cpu_info->vm_info.vm_end_vaddr = curr_cpu_info->vm_info.vm_start_vaddr + vm_size;
  curr_cpu_info->vm_info.vm_end_paddr = curr_cpu_info->vm_info.vm_start_paddr + vm_size;

  if (curr_cpu_info->vm_info.vm_end_paddr != curr_cpu_info->vm_info.vm_start_paddr + vm_size) {
    panic ("PM: Computed two different VM end addresses %x & %x\n", curr_cpu_info->vm_info.vm_end_paddr, curr_cpu_info->vm_info.vm_start_paddr + vm_size);
  }

  if (old_pcode)   free_mem_pages (old_pcode);
  if (old_pdata)   free_mem_pages (old_pdata);
  if (old_prodata) free_mem_pages (old_prodata);
  if (old_pbss)    free_mem_pages (old_pbss);
  if (old_pstack)  free_mem_pages (old_pstack);
  free_page_tables (old_pgtb);
  free_page_tables (old_ept);
#if 0
  DEBUG ("code p = %x v = %x\ndata p = %x v = %x\nrodata p = %x v = %x\nbss p = %x v = %x\nstack p = %x v = %x\n", 0xF, curr_cpu_info->vm_info.vm_code_paddr, curr_cpu_info->vm_info.vm_code_vaddr,
      curr_cpu_info->vm_info.vm_data_paddr, curr_cpu_info->vm_info.vm_data_vaddr,
      curr_cpu_info->vm_info.vm_rodata_paddr, curr_cpu_info->vm_info.vm_rodata_vaddr,
      curr_cpu_info->vm_info.vm_bss_paddr, curr_cpu_info->vm_info.vm_bss_vaddr,
      curr_cpu_info->vm_info.vm_stack_paddr, curr_cpu_info->vm_info.vm_stack_vaddr);
  DEBUG ("========================\n", 0xF);
#endif
}
/* ================================================= */
static pid_t
local_exec (struct cpu_info * const info, const struct exec_ipc * const exec_args)
{
  aint i;
  aint argc;
  aphys_addr_t *msg_data;
  aphys_addr_t stack_phys_off;
  avirt_addr_t stack_virt_off;
  avirt_addr_t argv_virt_ptr;
  avirt_addr_t *argv_phys_ptr;
  aphys_addr_t elf;

  /* Ask FS to read the ELF executable file */
  msg_send (FS, LOAD_IPC, exec_args->path, sizeof(char*));
  msg_receive (FS);

  msg_data = (phys_addr_t*)cpuinfo->msg_input[FS].data;
  elf = *msg_data;

  /* TODO: Remove the created channels if exec fails */

  if (elf == 0) {
    memcpy(&info->vm_info.vm_regs, (void*)exec_args->registers, sizeof(struct regs));
    return -1;
  }

  /*
   * Arguments are currently on the address space of calling process. Since after
   * exec, this process would disappear, we have to temporarily save these arguments
   * in a buffer. Since they are saved in a virtualized address space, w have to use
   * physical addresses to save them.
   */
  strcpy (arg_vector[0], exec_args->path);  /* Name of program, the first argument */
  for (i = 1; i < MAX_ARGV; i++) {
    char *curr_phys_arg __aligned (0x10);
    /* Check for the end of arguments */
    if (exec_args->argv == NULL || exec_args->argv[i - 1] == NULL) {
      break;
    }

    curr_phys_arg  = (char *)virt2phys (info, (virt_addr_t)exec_args->argv[i - 1]);

    strcpy (arg_vector[i], curr_phys_arg);
  }

  argc = i;  /* Number of arguments */
  /* Parse ELF executable, and load the processes */
  load_elf(info, elf);
  /* Map memory for the new processes */
  ept_pmap (info);
  /* Pass arguments. We use stack to store arguments */
  stack_phys_off = virt2phys (info, (virt_addr_t)info->vm_info.vm_regs.rsp);
  stack_virt_off = (virt_addr_t)info->vm_info.vm_regs.rsp;
  for (i = 0; i < argc; i++) {
    stack_phys_off -= (strlen (arg_vector[i]) + 1);
    stack_virt_off -= (strlen (arg_vector[i]) + 1);
    argv_ptr[i] = stack_virt_off;
    strcpy ((char *)stack_phys_off, arg_vector[i]);
  }


  /*
   * Here we allocate an array of virtual addresses. This would be identical with
   * argv[*]. Namely an array of "char *". We need to versions of this, one with
   * virtual addresses, which is passed to the new process, and one with physical
   * address which is used here by PM to store the values in physical memory.
   */
  /* virtual starting address of argv */
  argv_virt_ptr = stack_virt_off - argc * sizeof (virt_addr_t *);
  /* physical starting address of argv */
  argv_phys_ptr = (phys_addr_t *)(stack_phys_off - argc * sizeof (virt_addr_t *));
  /* put all already saved virtual addresses of argv entries on stack */
  for (i = 0; i < argc; i++) {
    argv_phys_ptr[i] = (virt_addr_t)argv_ptr[i];
  }

  /* Process stack starts after the address argv is saved */
  info->vm_info.vm_regs.rsp = (virt_addr_t)(argv_virt_ptr - sizeof (uint64_t));
  info->vm_info.vm_regs.rdx = (phys_addr_t)info;
  info->vm_info.vm_regs.rdi = argc;
  info->vm_info.vm_regs.rsi = argv_virt_ptr;
  return 0;
}
/* ================================================= */
/* XXX This code is very similar in FS, if any modification is done here
 *     it should probably be replaced also in FS.
 */
static int
close_channel(int fd, int from)
{
  virt_addr_t cnl_offset = get_cpu_info(from)->vm_info.vm_fds[fd].offset;
  struct channel *cnl = (struct channel *)cnl_offset;
  uint64_t msg = CLOSE_CHANNEL;
  int dst;

  memcpy((void*)cnl->data, &msg, sizeof (uint64_t));

  cnl_send(cnl);
  cnl_receive((virt_addr_t)cnl);

  local_channel_close(cnl_offset);

  get_cpu_info(from)->vm_info.vm_fds[fd].offset = 0;

  /* TODO: Find a better way to save the dst, maybe in the fd structure */
  switch (fd) {
    case 0:
      dst = KBD;
      break;
    case 1:
      dst = CONSOLE;
      break;
    default: /* If it is not KBD nor CONSOLE the only driver available is JUNK now */
      dst = JUNK;
  }

  const int r = 0;
  /*
   * Leave FS here, I know it's a kind of man-in-the-middle attack, but this way
   * the driver can just wait from a message from FS instead from ANY.
   */
  msg_reply (FS, dst, CHANNEL_CLOSE_IPC, &r, sizeof (int));

  return 0;
}
/* ================================================= */
static void
local_exit (struct cpu_info * const info, const struct exit_ipc * const exit_args)
{
  int i;
  struct cpu_info *parent_info;

  if (unlikely (info == NULL)) {
    DEBUG ("Exit: No cpu info provided!\n", 0x4);
    halt ();
  }


  if (likely (info->vm_info.vm_code_paddr))
    free_mem_pages (info->vm_info.vm_code_paddr);
  if (info->vm_info.vm_data_paddr)
    free_mem_pages (info->vm_info.vm_data_paddr);
  if (info->vm_info.vm_rodata_paddr)
    free_mem_pages (info->vm_info.vm_rodata_paddr);
  if (info->vm_info.vm_bss_paddr)
    free_mem_pages (info->vm_info.vm_bss_paddr);
  if (likely (info->vm_info.vm_stack_paddr))
    free_mem_pages (info->vm_info.vm_stack_paddr);
  free_page_tables (info->vm_info.vm_page_tables);
  free_page_tables (info->vm_info.vm_ept);

  parent_info = get_cpu_info (info->vm_info.vm_parent);
  if (unlikely (!parent_info)) {
    panic ("PM (local_exit): failed to get cpu information!");
  }

  if (likely (info->vm_info.vm_fds)) {
    /* Free pages allocated for channels */
    for (i = 0; i < MAX_FD; i++) {
      if (info->vm_info.vm_fds[i].type == TYPE_CHAR) {
        close_channel(i, info->cpuid);
      }
    }
    free_mem_pages ((phys_addr_t)info->vm_info.vm_fds);
    info->vm_info.vm_fds = NULL;
  }

  if (unlikely (info->vm_info.vm_channels)) {
    free_mem_pages ((virt_addr_t)info->vm_info.vm_channels);
    info->vm_info.vm_channels = 0;
  }

  /*
   * TODO:
   *     Unmap memory allocated for channels in driver side
   */

  if (parent_info->vm_info.vm_state == WAIT_CHLD) {
    struct waitpid_reply wait_reply;

    info->vmm_info.vmm_has_vm = false;

    wait_reply.child_pid = info->cpuid;
    wait_reply.status = exit_args->status;

    msg_reply (PM, parent_info->cpuid, WAITPID_IPC, &wait_reply, sizeof (struct waitpid_reply));
  } else {
    info->vm_info.vm_exit_status = exit_args->status;
  }
}
/* ================================================= */
static void
local_waitpid (struct cpu_info * const info,
               const struct waitpid_ipc * const waitpid_args,
               struct waitpid_reply *reply)
{
  struct cpu_info *child;

  if (unlikely (info == NULL)) {
    DEBUG ("Exit: No cpu info provided!\n", 0x4);
    halt ();
  }

  if (unlikely (waitpid_args->wait_for < -1 ||
                waitpid_args->wait_for > MAX_CPUS)) {
    DEBUG ("PM %s: waiting for invalid PID %d\n", 0x4, __func__, waitpid_args->wait_for);
    reply->child_pid = -1;
    return;
  }


  if (waitpid_args->wait_for != -1) {
    child = get_cpu_info (waitpid_args->wait_for);
    if (!child) {
      DEBUG ("PM: %s: Couldn't find child %d\n", 0x4, __func__, waitpid_args->wait_for);
      reply->child_pid = -1;
      return;
    }

    if (info->cpuid != child->vm_info.vm_parent) {
      reply->child_pid = -1;
      return;
    }
    if (child->vm_info.vm_state != ZOMBIE) {
      child = NULL;
    }
  } else {
    int i;
    for (i = ALWAYS_BUSY; i < MAX_CPUS; i++) {
      child = get_cpu_info (i);
      if (unlikely (!child)) {
        panic ("PM %s: Failed to get cpu information!\n", __func__);
      }

      if (child->vm_info.vm_parent == info->cpuid &&
          child->vm_info.vm_state == ZOMBIE) {
        break;
      }
    }
    /* NO child Found */
    if (i == MAX_CPUS) {
      child = NULL;
    }
  }

  if (child) {

    reply->child_pid = child->cpuid;
    reply->status = child->vm_info.vm_exit_status;

    child->vmm_info.vmm_has_vm = false;  /* No VM any more!! */
    return;
  } else {
    info->vm_info.vm_state = WAIT_CHLD;
    reply->child_pid = 0;
    return;
  }
}

/* ================================================= */
void
vm_main (void)
{
  interrupt_init();
  pm_init ();

  int i;
  for (i = 1; i < cpuinfo->ncpus; i++) {
    msg_reply (PM, i, 1, NULL, 0);
  }
#if 0
  int i;
  for (i = 0 ; i < cpuinfo->cpuid; i++) DEBUG ("\n", 0x7);
  DEBUG ("PM: My info is in addr = %d\n", 0xD, cpuinfo->cpuid);
  halt ();
#endif
  while (1) {
    struct message *m __aligned (0x10) = msg_check();
    struct waitpid_reply wait_reply;
    struct channel_close_ipc *ccipc;
    pid_t r;
    phys_addr_t channel;

    switch(m->number) {
      case FORK_IPC:
        r = local_fork (get_cpu_info (m->from), (struct fork_ipc *)m->data);
        /* Reply to the child */
        if (r != -1) {
          msg_reply (PM, r, FORK_IPC, &r, sizeof (pid_t));
        }
        /* Reply to the parent */
        msg_reply(PM, m->from, FORK_IPC, &r, sizeof(pid_t));
        break;
      case EXEC_IPC:
        r = local_exec (get_cpu_info (m->from), (struct exec_ipc *)m->data);
        msg_reply (PM, m->from, EXEC_IPC, &r, sizeof (pid_t));
        break;
      case EXIT_IPC:
        local_exit (get_cpu_info (m->from), (struct exit_ipc *)m->data);
        break;
      case WAITPID_IPC:
        local_waitpid (get_cpu_info (m->from), (struct waitpid_ipc *)m->data, &wait_reply);
        if (wait_reply.child_pid != 0) {
          msg_reply (PM, m->from, WAITPID_IPC, &wait_reply, sizeof (struct waitpid_reply));
        }
        break;
      case CHANNEL_IPC:
        channel = local_channel ((struct channel_ipc *)m->data);
        msg_reply (PM, m->from, CHANNEL_IPC, &channel, sizeof (phys_addr_t));
        break;
      case CHANNEL_CLOSE_IPC:
	ccipc = (struct channel_close_ipc *)m->data;
	local_channel_close (ccipc->cnl);
	r = 0;
	msg_reply (PM, m->from, CHANNEL_CLOSE_IPC, &r, sizeof (pid_t));
	break;
      default:
        DEBUG ("PM: Warning, unknown request %d from %d\n", 0xD, m->number, m->from);
    }
  }

  while (1) {__asm__ __volatile__ ("cli;hlt;\n\t");}
}

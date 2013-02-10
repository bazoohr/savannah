#include <cdef.h>
#include <dev/pic.h>
#include <types.h>
#include <printk.h>
#include <console.h>
#include <const.h>
#include <interrupt.h>
#include <asmfunc.h>
#include <memory.h>
#include <dev/keyboard.h>
#include <mp.h>
#include <dev/ioapic.h>
#include <dev/lapic.h>
#include <cpu.h>
#include <string.h>
#include <ipc.h>
#include <pm.h>
#include <fs.h>
#include <pm_args.h>
#include <config.h>
#include <pmap.h>
#include <elf.h>
#include <panic.h>
/* ================================================= */
#define ALWAYS_BUSY (NUMBER_SERVERS + NUMBER_USER_VMS)
/* ================================================= */
#define BUSY true
#define FREE false
/* ================================================= */
static bool vm_table[MAX_CPUS];
/* ================================================= */
static pid_t
find_free_vm (void)
{
  int i;
  for (i = ALWAYS_BUSY; i < MAX_CPUS; i++) {
    if (vm_table[i] == FREE) {
      return i;
    }
  }
  return MAX_CPUS + 1;
}
/* ================================================= */
static void
pm_init (void)
{
  int i;
  struct pm_args *pm_arguments;

  for (i = 0; i < ALWAYS_BUSY; i++) {
    vm_table[i] = BUSY;
  }

  for (i = ALWAYS_BUSY; i < MAX_CPUS; i++) {
    vm_table[i] = FREE;
  }

  pm_arguments = (struct pm_args*)cpuinfo->vm_args;
  memory_init (pm_arguments->last_used_addr, pm_arguments->memory_size);
}
/* ================================================= */
struct cpu_info *
get_cpu_info (cpuid_t cpuid)
{
  if (cpuid > MAX_CPUS) {
    return NULL;
  }

  return (struct cpu_info *)((phys_addr_t)cpuinfo + cpuid * _4KB_);
}
/* ================================================= */
static void
ept_pmap (struct cpu_info *child_cpu_info)
{
  map_memory (&child_cpu_info->vm_info.vm_page_tables,
      0, (virt_addr_t)((virt_addr_t)_1GB_ * 4),
      0,
      _1GB_,
      VM_PAGE_NORMAL, MAP_NEW);

  EPT_map_memory (&child_cpu_info->vm_info.vm_ept,
      0, _1MB_,
      0,
      USER_VMS_PAGE_SIZE,
      EPT_PAGE_READ | EPT_PAGE_WRITE, MAP_NEW);  /* XXX: Why do we need write access here? */

  EPT_map_memory (&child_cpu_info->vm_info.vm_ept,
      child_cpu_info->vm_info.vm_page_tables, child_cpu_info->vm_info.vm_page_tables + 2 * PAGE_TABLE_SIZE,
      child_cpu_info->vm_info.vm_page_tables,
      USER_VMS_PAGE_SIZE,
      EPT_PAGE_READ, MAP_UPDATE);
  EPT_map_memory (&child_cpu_info->vm_info.vm_ept,
      (phys_addr_t)child_cpu_info, (phys_addr_t)child_cpu_info + _4KB_,
      (phys_addr_t)child_cpu_info,
      USER_VMS_PAGE_SIZE,
      EPT_PAGE_READ, MAP_UPDATE);
  EPT_map_memory (&child_cpu_info->vm_info.vm_ept,
      (phys_addr_t)child_cpu_info->msg_input, (phys_addr_t)child_cpu_info->msg_input + _4KB_,
      (phys_addr_t)child_cpu_info->msg_input,
      USER_VMS_PAGE_SIZE,
      EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);
  EPT_map_memory (&child_cpu_info->vm_info.vm_ept,
      (phys_addr_t)child_cpu_info->msg_output, (phys_addr_t)child_cpu_info->msg_output + _4KB_,
      (phys_addr_t)child_cpu_info->msg_output,
      USER_VMS_PAGE_SIZE,
      EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);
  EPT_map_memory (&child_cpu_info->vm_info.vm_ept,
      (phys_addr_t)child_cpu_info->msg_ready, (phys_addr_t)child_cpu_info->msg_ready + _4KB_,
      (phys_addr_t)child_cpu_info->msg_ready,
      USER_VMS_PAGE_SIZE,
      EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);

  if (child_cpu_info->vm_info.vm_code_size > 0) {
    EPT_map_memory (&child_cpu_info->vm_info.vm_ept,
        child_cpu_info->vm_info.vm_code_vaddr, child_cpu_info->vm_info.vm_code_vaddr + child_cpu_info->vm_info.vm_code_size,
        child_cpu_info->vm_info.vm_code_paddr,
        USER_VMS_PAGE_SIZE,
        EPT_PAGE_READ | EPT_PAGE_EXEC, MAP_UPDATE);
  }
  if (child_cpu_info->vm_info.vm_data_size > 0) {
    EPT_map_memory (&child_cpu_info->vm_info.vm_ept,
        child_cpu_info->vm_info.vm_data_vaddr, child_cpu_info->vm_info.vm_data_vaddr + child_cpu_info->vm_info.vm_data_size,
        child_cpu_info->vm_info.vm_data_paddr,
        USER_VMS_PAGE_SIZE,
        EPT_PAGE_READ | EPT_PAGE_EXEC, MAP_UPDATE);
  }
  if (child_cpu_info->vm_info.vm_rodata_size > 0) {
    EPT_map_memory (&child_cpu_info->vm_info.vm_ept,
        child_cpu_info->vm_info.vm_rodata_vaddr, child_cpu_info->vm_info.vm_rodata_vaddr + child_cpu_info->vm_info.vm_rodata_size,
        child_cpu_info->vm_info.vm_rodata_paddr,
        USER_VMS_PAGE_SIZE,
        EPT_PAGE_READ, MAP_UPDATE);
  }
  if (child_cpu_info->vm_info.vm_bss_size > 0) {
    EPT_map_memory (&child_cpu_info->vm_info.vm_ept,
        child_cpu_info->vm_info.vm_bss_vaddr, child_cpu_info->vm_info.vm_bss_vaddr + child_cpu_info->vm_info.vm_bss_size,
        child_cpu_info->vm_info.vm_bss_paddr,
        USER_VMS_PAGE_SIZE,
        EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);
  }
  if (child_cpu_info->vm_info.vm_stack_size > 0) {
    EPT_map_memory (&child_cpu_info->vm_info.vm_ept,
        child_cpu_info->vm_info.vm_stack_vaddr, child_cpu_info->vm_info.vm_stack_vaddr + child_cpu_info->vm_info.vm_stack_size,
        child_cpu_info->vm_info.vm_stack_paddr,
        USER_VMS_PAGE_SIZE,
        EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);
  }
}
static pid_t
local_fork (struct cpu_info *info, struct fork_ipc *fork_args)
{
  pid_t child_vm;
  cpuid_t parent_id;
  struct cpu_info *child_cpu_info;
  struct cpu_info *parent_cpu_info;

  if (info == NULL) {
    return -1;
  }

  parent_id = info->cpuid;

  child_vm = find_free_vm ();
  if (child_vm > MAX_CPUS) {
    return -1;
  }

  child_cpu_info = get_cpu_info (child_vm);
  parent_cpu_info = get_cpu_info (parent_id);

  child_cpu_info->vm_info.vm_code_paddr = (phys_addr_t)malloc_align (parent_cpu_info->vm_info.vm_code_size, USER_VMS_PAGE_SIZE);
  child_cpu_info->vm_info.vm_code_vaddr = parent_cpu_info->vm_info.vm_code_vaddr;
  child_cpu_info->vm_info.vm_code_size  = parent_cpu_info->vm_info.vm_code_size;
  memcpy ((void*)child_cpu_info->vm_info.vm_code_paddr, (void*)parent_cpu_info->vm_info.vm_code_paddr, parent_cpu_info->vm_info.vm_code_size);

  child_cpu_info->vm_info.vm_data_paddr = (phys_addr_t)malloc_align (parent_cpu_info->vm_info.vm_data_size, USER_VMS_PAGE_SIZE);
  child_cpu_info->vm_info.vm_data_vaddr = parent_cpu_info->vm_info.vm_data_vaddr;
  child_cpu_info->vm_info.vm_data_size  = parent_cpu_info->vm_info.vm_data_size;
  memcpy ((void*)child_cpu_info->vm_info.vm_data_paddr, (void*)parent_cpu_info->vm_info.vm_data_paddr, parent_cpu_info->vm_info.vm_data_size);

  child_cpu_info->vm_info.vm_rodata_paddr = (phys_addr_t)malloc_align (parent_cpu_info->vm_info.vm_rodata_size, USER_VMS_PAGE_SIZE);
  child_cpu_info->vm_info.vm_rodata_vaddr = parent_cpu_info->vm_info.vm_rodata_vaddr;
  child_cpu_info->vm_info.vm_rodata_size  = parent_cpu_info->vm_info.vm_rodata_size;
  memcpy ((void*)child_cpu_info->vm_info.vm_rodata_paddr, (void*)parent_cpu_info->vm_info.vm_rodata_paddr, parent_cpu_info->vm_info.vm_rodata_size);

  child_cpu_info->vm_info.vm_bss_paddr = (phys_addr_t)calloc_align (sizeof (uint8_t), parent_cpu_info->vm_info.vm_bss_size, USER_VMS_PAGE_SIZE);
  child_cpu_info->vm_info.vm_bss_vaddr = parent_cpu_info->vm_info.vm_bss_vaddr;
  child_cpu_info->vm_info.vm_bss_size  = parent_cpu_info->vm_info.vm_bss_size;
  memcpy ((void*)child_cpu_info->vm_info.vm_bss_paddr, (void*)parent_cpu_info->vm_info.vm_bss_paddr, parent_cpu_info->vm_info.vm_bss_size);

  child_cpu_info->vm_info.vm_stack_paddr = (phys_addr_t)malloc_align (parent_cpu_info->vm_info.vm_stack_size, USER_VMS_PAGE_SIZE);
  child_cpu_info->vm_info.vm_stack_vaddr = parent_cpu_info->vm_info.vm_stack_vaddr;
  child_cpu_info->vm_info.vm_stack_size  = parent_cpu_info->vm_info.vm_stack_size;
  memcpy ((void*)child_cpu_info->vm_info.vm_stack_paddr, (void*)parent_cpu_info->vm_info.vm_stack_paddr, parent_cpu_info->vm_info.vm_stack_size);

  /*
   * VM's page tables are already build in boot/stage2
   * so we only need to map ept
   */
  ept_pmap (child_cpu_info);

  memcpy ((void *)&child_cpu_info->vm_info.vm_regs, (void *)fork_args->register_array_paddr, sizeof (struct regs));

  /*
   * TODO:
   * variable  cpuinfo might not be used in bss section. So here, code should be defensive, and check to find out
   * exactly in which section cpuinfo is stored! At the present time, I just write the code for section BSS to get
   * the job done, but this is NOT the proper way to do this.
   */
  size_t offset = virt2phys (parent_cpu_info, fork_args->cpuinfo_vaddr) - parent_cpu_info->vm_info.vm_bss_paddr;
  struct cpu_info **child_cpu_info_ptr = (struct cpu_info **)((phys_addr_t)child_cpu_info->vm_info.vm_bss_paddr + offset);
  *child_cpu_info_ptr = child_cpu_info;

  child_cpu_info->vm_info.vm_regs.rax = 0;

  return child_vm;
}
/* ================================================= */
static void
parse_elf (struct cpu_info *curr_cpu_info, phys_addr_t elf)
{
  size_t vm_size;     /* VMM's Code + data + rodata + bss + stack */
  int ph_num;          /* VMM's number of program headers */
  int i;
  Elf64_Ehdr *elf_hdr;  /* Start address of executable */
  Elf64_Phdr *s;

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
  elf_hdr = (Elf64_Ehdr*)elf;  /* Start address of executable */

  ph_num = elf_hdr->e_phnum;    /* Number of program headers in ELF executable */
  if (ph_num != 4) {
    cprintk ("ELF executable contains %d sections!\n", 0x4, ph_num);
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
    phys_addr_t section_src_paddr = 0;
    phys_addr_t section_dst_paddr = 0;
    size_t section_size;

    section_src_paddr = elf + s->p_offset;  /* Address of section in executable */
    section_size = (size_t)s->p_memsz;               /* Section size */
    if (section_size > 0) {
      /* Address of section when loaded in ram */
      section_dst_paddr = (phys_addr_t)malloc_align (section_size, USER_VMS_PAGE_SIZE);
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
    curr_cpu_info->vm_info.vm_bss_paddr = (phys_addr_t)malloc_align (_2MB_, USER_VMS_PAGE_SIZE);
    curr_cpu_info->vm_info.vm_bss_size = s->p_memsz;
    memset ((void*)curr_cpu_info->vm_info.vm_bss_paddr, 0, s->p_memsz);
  }
  /*
   * Here we want to reserve some space for the stack. Stack should be
   * 16 byte aligned!
   */
  curr_cpu_info->vm_info.vm_stack_vaddr = ALIGN ((virt_addr_t)s->p_vaddr + s->p_memsz, USER_VMS_PAGE_SIZE);
  curr_cpu_info->vm_info.vm_stack_size = _2MB_;
  curr_cpu_info->vm_info.vm_stack_paddr = (phys_addr_t)malloc_align (_2MB_, USER_VMS_PAGE_SIZE);
  curr_cpu_info->vm_info.vm_regs.rsp = curr_cpu_info->vm_info.vm_stack_vaddr + curr_cpu_info->vm_info.vm_stack_size;

  vm_size = (curr_cpu_info->vm_info.vm_stack_paddr + curr_cpu_info->vm_info.vm_stack_size) - curr_cpu_info->vm_info.vm_start_paddr;

  curr_cpu_info->vm_info.vm_end_vaddr = curr_cpu_info->vm_info.vm_start_vaddr + vm_size;
  curr_cpu_info->vm_info.vm_end_paddr = curr_cpu_info->vm_info.vm_start_paddr + vm_size;

  if (curr_cpu_info->vm_info.vm_end_paddr != curr_cpu_info->vm_info.vm_start_paddr + vm_size) {
    panic ("PM: Computed two different VM end addresses %x & %x\n", curr_cpu_info->vm_info.vm_end_paddr, curr_cpu_info->vm_info.vm_start_paddr + vm_size);
  }

  cprintk ("code p = %x v = %x\ndata p = %x v = %x\nrodata p = %x v = %x\nbss p = %x v = %x\nstack p = %x v = %x\n", 0xF, curr_cpu_info->vm_info.vm_code_paddr, curr_cpu_info->vm_info.vm_code_vaddr,
      curr_cpu_info->vm_info.vm_data_paddr, curr_cpu_info->vm_info.vm_data_vaddr,
      curr_cpu_info->vm_info.vm_rodata_paddr, curr_cpu_info->vm_info.vm_rodata_vaddr,
      curr_cpu_info->vm_info.vm_bss_paddr, curr_cpu_info->vm_info.vm_bss_vaddr,
      curr_cpu_info->vm_info.vm_stack_paddr, curr_cpu_info->vm_info.vm_stack_vaddr);
  cprintk ("========================\n", 0xF);
}
static pid_t
local_exec (struct cpu_info *info, struct exec_ipc *exec_args)
{
  msg_send (FS, LOAD_IPC, exec_args->path, sizeof(char*));
  msg_receive ();

  phys_addr_t elf = *(phys_addr_t *)cpuinfo->msg_input->data;

  if (elf == 0) {
    return -1;
  }

  cprintk("%c %c %c %c\n", 0xF, ((char*)elf)[0], ((char*)elf)[1], ((char*)elf)[2], ((char*)elf)[3]);

  parse_elf(info, elf);

  ept_pmap (info);

  return 0;
}
/* ================================================= */
void
vm_main (void)
{
  int i;
  con_init ();

  pm_init ();

  for (i = 0 ; i < cpuinfo->cpuid; i++) printk("\n");

  cprintk ("PM: My info is in addr = %d\n", 0xD, cpuinfo->cpuid);

  while (1) {
    struct message *m = msg_check();
    pid_t r;


    switch(m->number) {
      case FORK_IPC:
          r = local_fork (get_cpu_info (m->from), (struct fork_ipc *)m->data);
	  /* Reply to the child */
	  if (r != -1) {
		  msg_reply (r, FORK_IPC, &r, sizeof (pid_t));
	  }
	  /* Reply to the parent */
	  msg_reply(m->from, FORK_IPC, &r, sizeof(pid_t));
          break;
      case EXEC_IPC:
	  r = local_exec (get_cpu_info (m->from), (struct exec_ipc *)m->data);
	  msg_reply (m->from, EXEC_IPC, &r, sizeof (pid_t));
	  break;
      default:
        cprintk("PM: Warning, unknown request %d from %d\n", 0xD, m->number, m->from);
    }
  }

  while (1) {__asm__ __volatile__ ("cli;hlt;\n\t");}
}

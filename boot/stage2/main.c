#include <cdef.h>
#include <dev/pic.h>
#include <types.h>
#include <boot_stage2_args.h>
#include <printk.h>
#include <console.h>
#include <const.h>
#include <interrupt.h>
#include <asmfunc.h>
#include <dev/keyboard.h>
#include <mp.h>
#include <dev/ioapic.h>
#include <dev/lapic.h>
#include <cpu.h>
#include <elf.h>
#include <string.h>
#include <panic.h>
#include <page.h>
#include <pmap.h>
#include <message.h>
#include <memory.h>
#include <macro.h>
#include <config.h>
/* ========================================== */
phys_addr_t vmx_data_structure_pool;
struct message *msg_input;
struct message *msg_output;
bool *msg_ready;
/* ========================================== */
phys_addr_t
get_vmxon_ptr (cpuid_t cpuid)
{
  if (cpuid > get_ncpus ()) {
    panic ("get_vmxon_ptr: cpuid out of rainge!");
  }

  return vmx_data_structure_pool + cpuid * 0x2000;
}
/* ========================================== */
phys_addr_t
get_vmcs_ptr (cpuid_t cpuid)
{
  if (cpuid > get_ncpus ()) {
    panic ("get_vmxon_ptr: cpuid out of rainge!");
  }

  return vmx_data_structure_pool + cpuid * 0x2000 + 0x1000;
}
/* ========================================== */
struct message *
get_msg_input (cpuid_t cpuid)
{
  if (cpuid > get_ncpus ()) {
    panic ("get_vmxon_ptr: cpuid out of rainge!");
  }

  return msg_input;
}
struct message *
get_msg_output (cpuid_t cpuid)
{
  if (cpuid > get_ncpus ()) {
    panic ("get_vmxon_ptr: cpuid out of rainge!");
  }

  return msg_output;
}
bool *
get_msg_ready (cpuid_t cpuid)
{
  if (cpuid > get_ncpus ()) {
    panic ("get_vmxon_ptr: cpuid out of rainge!");
  }

  return msg_ready;
}
/* ========================================== */
#define VIRT2PHYS(vaddr) (\
    (phys_addr_t)(\
      ((phys_addr_t)(curr_vmm_phys_addr))\
      +                                  \
      ((vaddr) - ((virt_addr_t)curr_cpu_info->vmm_start_vaddr))\
      )\
    )
/* ========================================== */
static void
load_all_vmms (phys_addr_t vmm_elf_addr, phys_addr_t boot_stage2_end_addr)
{
  Elf64_Ehdr *elf_hdr;  /* Start address of executable */
  Elf64_Phdr *s;

  phys_addr_t curr_vmm_phys_addr;

  size_t vmm_size;     /* VMM's Code + data + rodata + bss + stack */
  int ph_num;          /* VMM's number of program headers */
  int i;
  cpuid_t curr_cpu;
  struct cpu_info *curr_cpu_info;

  /* TODO:
   *    Replace constant value 0x400000 with a meaningful MACRO
   * XXX:
   *    I'm going to use variables curr_vmm_phys_addr and curr_cpu_info in the macro
   *    VIRT2PHYS(). So be careful in using/altering/removing this
   *    variable. This is a bad hack, but it makes our code a lot
   *    easier to read.
   *    Before Using VIRT2PHYS() macro, make sure
   *    curr_vmm_phys_addr and curr_cpu_info are assigned with proper value. Otherwise
   *    everything will break.
   *    To minimize the threat, I'm going to define VIRT2PHYS Macro
   *    after assignment of curr_vmm_phys_addr. Don't move or reorder them!
   */
  for (curr_cpu = 0; curr_cpu < get_ncpus(); curr_cpu++) {
    curr_vmm_phys_addr = (phys_addr_t)malloc_align (VMM_MAX_SIZE, VMM_MAX_SIZE);
    if (curr_vmm_phys_addr == 0) {
      panic ("Second Boot Stage: Memory Allocation Failed! Line %d\n", __LINE__);
    }

    curr_cpu_info = get_cpu_info (curr_cpu);
    curr_cpu_info->vmm_start_paddr = curr_vmm_phys_addr;
    cprintk ("curr vmm %x curr cpu info %x\n", 0xF, curr_vmm_phys_addr, curr_cpu_info);

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
    elf_hdr = (Elf64_Ehdr*)vmm_elf_addr;  /* Start address of executable */

    ph_num = elf_hdr->e_phnum;    /* Number of program headers in ELF executable */
    if (ph_num != 4) {
      cprintk ("ELF executable contains %d sections!\n", 0x4, ph_num);
      panic ("Unexpected VMM Header!\n");
    }

    s = (Elf64_Phdr*)((uint8_t*)vmm_elf_addr + elf_hdr->e_phoff);
    curr_cpu_info->vmm_start_vaddr = (virt_addr_t)s->p_vaddr;
    /* Why Upper bound is ph_num - 1?
     * Because in the VMM ELF executable, last section is .bss which does not have any byes.
     * So we don't need to copy any bytes from this section into memory. But we do need to allocate
     * enough memory for this section when we load the executable, and we
     * also need to zero out allocated memory. That's why we put .bss section at the
     * end of executable.
     */
    for (i = 0; i < ph_num - 1; i++) {
      phys_addr_t section_src_paddr;
      phys_addr_t section_dst_paddr;
      size_t section_size;

      section_src_paddr = vmm_elf_addr + s->p_offset;  /* Address of section in executable */
      section_dst_paddr = VIRT2PHYS (s->p_vaddr);      /* Address of section when loaded in ram */
      section_size = (size_t)s->p_memsz;               /* Section size */

      if (section_size > 0) {
        memcpy ((void*)section_dst_paddr, (void*)section_src_paddr, section_size);
      }
      s++;    /* Go to next section */
    }
    /* Last section is bss. We zero out this section */
    memset ((void*)VIRT2PHYS (s->p_vaddr), 0, s->p_memsz);
    /*
     * Here we want to reserve some space for the stack. Stack should be
     * 16 byte aligned!
     */
    curr_cpu_info->vmm_vstack = ALIGN ((virt_addr_t)s->p_vaddr + s->p_memsz, 16) + VMM_STACK_SIZE;

    vmm_size = curr_cpu_info->vmm_vstack - curr_cpu_info->vmm_start_vaddr;

    curr_cpu_info->vmm_end_vaddr = curr_cpu_info->vmm_start_vaddr + vmm_size;

    if (vmm_size > VMM_MAX_SIZE) {
      panic ("Second Boot Stage: vmm is too large!");
    }

    curr_cpu_info->vmm_end_paddr = curr_cpu_info->vmm_start_paddr + vmm_size;

    /* Message Box */
    curr_cpu_info->msg_input  = get_msg_input(curr_cpu);
    curr_cpu_info->msg_output = get_msg_output(curr_cpu);
    curr_cpu_info->msg_ready  = get_msg_ready(curr_cpu);
  } /* For */
  
  for (curr_cpu = 0; curr_cpu < get_ncpus (); curr_cpu++) {
    curr_cpu_info = get_cpu_info (curr_cpu);

    //cprintk ("Last MAPED ADDRESS = %x\n", 0xF, (phys_addr_t)get_cpu_info (get_ncpus () - 1) + sizeof (struct cpu_info));
    map_memory (&curr_cpu_info->vmm_page_tables,
                 0, boot_stage2_end_addr,
                 0, 
                 _4KB_,
                 VMM_PAGE_NORMAL, MAP_NEW);

    map_memory (&curr_cpu_info->vmm_page_tables,
                 vmx_data_structure_pool, (phys_addr_t)msg_input,
                 vmx_data_structure_pool,
                 _4KB_,
                 VMM_PAGE_NORMAL, MAP_UPDATE);

    map_memory (&curr_cpu_info->vmm_page_tables,
                 curr_cpu_info->vmm_start_vaddr, curr_cpu_info->vmm_end_vaddr,
                 curr_cpu_info->vmm_start_paddr, 
                 _4KB_,
                 VMM_PAGE_NORMAL, MAP_UPDATE);

    EPT_map_memory (&curr_cpu_info->vmm_ept_tables,
                 0, curr_cpu_info->vmm_start_vaddr,
                 0, 
                 _1GB_,
                 VMM_PAGE_NORMAL, MAP_NEW);
  }
  /* =============================================== */
  /* XXX:
   *    After booting the system, NOT all cpus have
   *    VMs to run, therefore we have to somehow indicate
   *    which VMMs should launch a VM and which should not.
   *    if both start virtual & physical addresses of a VM
   *    are zero, VMM will not try to launch any VM.
   */
  for (i = NUMBER_SERVERS; i < get_ncpus (); i++) {
    (get_cpu_info (i))->vm_start_vaddr = 0;
    (get_cpu_info (i))->vm_start_paddr = 0;
  }
#undef VIRT2PHYS
}
/* ========================================== */
#define VIRT2PHYS(vaddr) (\
    (phys_addr_t)(\
      ((phys_addr_t)(curr_vm_phys_addr))\
      +\
      ((vaddr) - ((virt_addr_t)curr_cpu_info->vm_start_vaddr))\
      )\
    )
/* ========================================== */
static void
load_all_vms (phys_addr_t *vms_array, phys_addr_t boot_stage2_end_addr)
{
  Elf64_Ehdr *elf_hdr;  /* Start address of executable */
  Elf64_Phdr *s;

  phys_addr_t curr_vm_phys_addr;

  size_t vm_size;     /* VMM's Code + data + rodata + bss + stack */
  int ph_num;          /* VMM's number of program headers */
  int i;
  cpuid_t curr_cpu;
  struct cpu_info *curr_cpu_info;

  /* TODO:
   *    Replace constant value 0x400000 with a meaningful MACRO
   * XXX:
   *    I'm going to use variables curr_vmm_phys_addr and curr_cpu_info in the macro
   *    VIRT2PHYS(). So be careful in using/altering/removing this
   *    variable. This is a bad hack, but it makes our code a lot
   *    easier to read.
   *    Before Using VIRT2PHYS() macro, make sure
   *    curr_vmm_phys_addr and curr_cpu_info are assigned with proper value. Otherwise
   *    everything will break.
   *    To minimize the threat, I'm going to define VIRT2PHYS Macro
   *    after assignment of curr_vmm_phys_addr. Don't move or reorder them!
   */
  for (curr_cpu = 0; curr_cpu < NUMBER_SERVERS; curr_cpu++) {
    curr_vm_phys_addr = (phys_addr_t)malloc_align(VM_MAX_SIZE, VM_MAX_SIZE);
    if (curr_vm_phys_addr == 0) {
      panic ("Second Boot Stage: Memory Allocation Failed! Line %d\n", __LINE__);
    }

    curr_cpu_info = get_cpu_info (curr_cpu);
    curr_cpu_info->vm_start_paddr = curr_vm_phys_addr;

    cprintk ("curr vm %x\n", 0xF, curr_vm_phys_addr);
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
    elf_hdr = (Elf64_Ehdr*)vms_array[curr_cpu];  /* Start address of executable */

    ph_num = elf_hdr->e_phnum;    /* Number of program headers in ELF executable */
    if (ph_num != 4) {
      cprintk ("ELF executable contains %d sections!\n", 0x4, ph_num);
      panic ("Unexpected VM Header!\n");
    }

    s = (Elf64_Phdr*)((uint8_t*)vms_array[curr_cpu] + elf_hdr->e_phoff);
    curr_cpu_info->vm_start_vaddr = (virt_addr_t)s->p_vaddr;
    /* Why Upper bound is ph_num - 1?
     * Because in the VM ELF executable, last section is .bss which does not have any byes.
     * So we don't need to copy any bytes from this section into memory. But we do need to allocate
     * enough memory for this section when we load the executable, and we
     * also need to zero out allocated memory. That's why we put .bss section at the
     * end of executable.
     */
    for (i = 0; i < ph_num - 1; i++) {
      phys_addr_t section_src_paddr;
      phys_addr_t section_dst_paddr;
      size_t section_size;

      section_src_paddr = vms_array[curr_cpu] + s->p_offset;  /* Address of section in executable */
      section_dst_paddr = VIRT2PHYS (s->p_vaddr);      /* Address of section when loaded in ram */
      section_size = (size_t)s->p_memsz;               /* Section size */

      if (section_size > 0) {
        memcpy ((void*)section_dst_paddr, (void*)section_src_paddr, section_size);
      }
      s++;    /* Go to next section */
    }
    /* Last section is bss. We zero out this section */
    memset ((void*)VIRT2PHYS (s->p_vaddr), 0, s->p_memsz);
    /*
     * Here we want to reserve some space for the stack. Stack should be
     * 16 byte aligned!
     */
    curr_cpu_info->vm_vstack = ALIGN ((virt_addr_t)s->p_vaddr + s->p_memsz, 16) + VM_STACK_SIZE;

    curr_cpu_info->vm_vmxon_ptr = get_vmxon_ptr (curr_cpu_info->cpuid);
    curr_cpu_info->vm_vmcs_ptr  = get_vmcs_ptr  (curr_cpu_info->cpuid);

    vm_size = curr_cpu_info->vm_vstack - curr_cpu_info->vm_start_vaddr;

    curr_cpu_info->vm_end_vaddr = curr_cpu_info->vm_start_vaddr + vm_size;

    if (vm_size > VM_MAX_SIZE) {
      panic ("Second Boot Stage: vm is too large!");
    }

    curr_cpu_info->vm_end_paddr = curr_cpu_info->vm_start_paddr + vm_size;

  } /* For */

  for (curr_cpu = 0; curr_cpu < NUMBER_SERVERS; curr_cpu++) {
    curr_cpu_info = get_cpu_info (curr_cpu);

    map_memory (&curr_cpu_info->vm_page_tables,
                 0, boot_stage2_end_addr,
                 0,
                 _4KB_,
                 VM_PAGE_NORMAL, MAP_NEW);
    map_memory (&curr_cpu_info->vm_page_tables,
                 vmx_data_structure_pool, (phys_addr_t)msg_ready + get_ncpus() * sizeof(bool),
                 vmx_data_structure_pool,
                 _4KB_,
                 VM_PAGE_NORMAL, MAP_UPDATE);
    map_memory (&curr_cpu_info->vm_page_tables,
        curr_cpu_info->vm_start_vaddr, curr_cpu_info->vm_end_vaddr,
        curr_cpu_info->vm_start_paddr, 
        _4KB_,
        VM_PAGE_NORMAL, MAP_UPDATE);
  }
 
#undef VIRT2PHYS
}

void 
boot_stage2_main (struct boot_stage2_args *boot_args)
{
  phys_addr_t vms_array[] = {boot_args->pm_elf_addr, boot_args->fs_elf_addr};

  con_init ();
  cprintk ("boot_stage2_end_addr = %x\n", 0xB, boot_args->boot_stage2_end_addr);
  memory_init (boot_args->boot_stage2_end_addr, boot_args->sys_mem_size);

  map_memory (&(boot_args->boot_stage2_page_tables), 0xFEC00000, 0x100000000, 0xFEC00000, _2MB_, VMM_PAGE_UNCACHABLE, MAP_UPDATE);

  mp_init ();

  vmx_data_structure_pool = (phys_addr_t)malloc_align (get_ncpus () * 0x1000 * 2, 0x1000);
  cprintk ("vmx_data_structure_pool = %x\n", 0xB, vmx_data_structure_pool);
  msg_input = (struct message *)calloc_align (sizeof (uint8_t), get_ncpus() * sizeof(struct message), 16);
  cprintk ("msg_input = %x\n", 0xB, msg_input);
  msg_output = (struct message *)calloc_align (sizeof (uint8_t), get_ncpus() * sizeof(struct message), 16);
  cprintk ("msg_output = %x\n", 0xB, msg_output);
  msg_ready = (bool *)calloc_align (sizeof (uint8_t), get_ncpus() * sizeof(bool), 16);
  cprintk ("msg_ready = %x\n", 0xB, msg_ready);

  interrupt_init ();

  pic_init ();
  ioapic_init ();
  lapic_init();
  kbd_init ();
  load_all_vmms (boot_args->vmm_elf_addr, boot_args->boot_stage2_end_addr);
  load_all_vms (vms_array, boot_args->boot_stage2_end_addr);
  mp_bootothers ();
  __asm__ __volatile__ ("sti\n");

  for (;;);
}

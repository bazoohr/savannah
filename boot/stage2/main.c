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
#include <pm_args.h>
#include <config.h>
/* ========================================== */
struct message *msg_input;
struct message *msg_output;
bool *msg_ready;
/* ========================================== */
static struct message *
get_msg_input (cpuid_t cpuid)
{
  if (cpuid > get_ncpus ()) {
    panic ("get_vmxon_ptr: cpuid out of rainge!");
  }

  return (struct message *)((phys_addr_t)msg_input + cpuid * _4KB_);
}
static struct message *
get_msg_output (cpuid_t cpuid)
{
  if (cpuid > get_ncpus ()) {
    panic ("get_vmxon_ptr: cpuid out of rainge!");
  }

  return (struct message *)((phys_addr_t)msg_output + cpuid * _4KB_);
}
static bool *
get_msg_ready (cpuid_t cpuid)
{
  if (cpuid > get_ncpus ()) {
    panic ("get_vmxon_ptr: cpuid out of rainge!");
  }

  return (bool *)((phys_addr_t)msg_ready + cpuid * _4KB_);
}
/* ========================================== */
#define VIRT2PHYS(vaddr) (\
    (phys_addr_t)(\
      ((phys_addr_t)(curr_vmm_phys_addr))\
      +                                  \
      ((vaddr) - ((virt_addr_t)curr_cpu_info->vmm_info.vmm_start_vaddr))\
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
    curr_cpu_info->ncpus = get_ncpus ();

    /*
     * For servers, we are going to use 2MB pages (for performance and security reasons).
     * Therefore, the VMXPTR and VMCSPTR must be 2MB aligned.
     */
    curr_cpu_info->vm_info.vm_vmcs_ptr = (phys_addr_t)calloc_align (sizeof (uint8_t), _4KB_, _4KB_);
    curr_cpu_info->vm_info.vm_vmxon_ptr = (phys_addr_t)calloc_align (sizeof (uint8_t), _4KB_, _4KB_);

    curr_cpu_info->vmm_info.vmm_start_paddr = curr_vmm_phys_addr;
    //cprintk ("curr vmm %x curr cpu info %x\n", 0xF, curr_vmm_phys_addr, curr_cpu_info);

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
    curr_cpu_info->vmm_info.vmm_start_vaddr = (virt_addr_t)s->p_vaddr;
    curr_cpu_info->vmm_info.vmm_regs.rip = curr_cpu_info->vmm_info.vmm_start_vaddr;
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
    //cprintk ("BSS ADDRESS = %x SIZE = %d\n", 0xA, s->p_vaddr, s->p_memsz);
    memset ((void*)VIRT2PHYS (s->p_vaddr), 0, s->p_memsz);
    /*
     * Here we want to reserve some space for the stack. Stack should be
     * 16 byte aligned!
     */
    curr_cpu_info->vmm_info.vmm_stack_vaddr = ALIGN ((virt_addr_t)s->p_vaddr + s->p_memsz, 16);
    curr_cpu_info->vmm_info.vmm_stack_paddr = VIRT2PHYS (curr_cpu_info->vmm_info.vmm_stack_vaddr);
    curr_cpu_info->vmm_info.vmm_stack_size = VMM_STACK_SIZE;
    curr_cpu_info->vmm_info.vmm_regs.rsp = curr_cpu_info->vmm_info.vmm_stack_vaddr + curr_cpu_info->vmm_info.vmm_stack_size;

    vmm_size = (curr_cpu_info->vmm_info.vmm_stack_vaddr + curr_cpu_info->vmm_info.vmm_stack_size) - curr_cpu_info->vmm_info.vmm_start_vaddr;

    curr_cpu_info->vmm_info.vmm_end_vaddr = curr_cpu_info->vmm_info.vmm_start_vaddr + vmm_size;

    if (vmm_size > VMM_MAX_SIZE) {
      panic ("Second Boot Stage: vmm is too large!");
    }

    curr_cpu_info->vmm_info.vmm_end_paddr = curr_cpu_info->vmm_info.vmm_start_paddr + vmm_size;

#if 0
    cprintk ("BSS START = %x BSS_END = %x STACK_START = %x STACK_END = %x\n", 0x3, s->p_vaddr, s->p_vaddr + s->p_memsz,
        curr_cpu_info->vmm_info.vmm_stack_vaddr, curr_cpu_info->vmm_info.vmm_regs.rsp);
    cprintk ("P: BSS START = %x BSS_END = %x STACK_START = %x STACK_END = %x\n", 0x3, VIRT2PHYS(s->p_vaddr), VIRT2PHYS((s->p_vaddr + s->p_memsz)),
        curr_cpu_info->vmm_info.vmm_stack_paddr, curr_cpu_info->vmm_info.vmm_end_paddr);
#endif

    /* Message Box */
    curr_cpu_info->msg_input  = get_msg_input(curr_cpu);
    curr_cpu_info->msg_output = get_msg_output(curr_cpu);
    curr_cpu_info->msg_ready  = get_msg_ready(curr_cpu);
  } /* For */

  for (curr_cpu = 0; curr_cpu < get_ncpus (); curr_cpu++) {
    curr_cpu_info = get_cpu_info (curr_cpu);

    //cprintk ("Last MAPED ADDRESS = %x\n", 0xF, (phys_addr_t)get_cpu_info (get_ncpus () - 1) + sizeof (struct cpu_info));
    map_memory (&curr_cpu_info->vmm_info.vmm_page_tables,
                 0, boot_stage2_end_addr,
                 0,
                 _4KB_,
                 VMM_PAGE_NORMAL, MAP_NEW);
    //cprintk ("cpuinfo_table = %x curr_cpu_info %x cpuid - %d\n", 0x2, cpuinfo_table, curr_cpu_info, curr_cpu_info->cpuid);
    map_memory (&curr_cpu_info->vmm_info.vmm_page_tables,
                 curr_cpu_info->vm_info.vm_vmxon_ptr, curr_cpu_info->vm_info.vm_vmxon_ptr + _4KB_,
                 curr_cpu_info->vm_info.vm_vmxon_ptr,
                 _4KB_,
                 VMM_PAGE_NORMAL, MAP_UPDATE);
    map_memory (&curr_cpu_info->vmm_info.vmm_page_tables,
                 curr_cpu_info->vm_info.vm_vmcs_ptr, curr_cpu_info->vm_info.vm_vmcs_ptr + _4KB_,
                 curr_cpu_info->vm_info.vm_vmcs_ptr,
                 _4KB_,
                 VMM_PAGE_NORMAL, MAP_UPDATE);
    map_memory (&curr_cpu_info->vmm_info.vmm_page_tables,
                 (phys_addr_t)curr_cpu_info->msg_input, (phys_addr_t)curr_cpu_info->msg_input + _4KB_,
                 (phys_addr_t)curr_cpu_info->msg_input,
                 _4KB_,
                 VMM_PAGE_NORMAL, MAP_UPDATE);
    map_memory (&curr_cpu_info->vmm_info.vmm_page_tables,
                 (phys_addr_t)curr_cpu_info->msg_output, (phys_addr_t)curr_cpu_info->msg_output + _4KB_,
                 (phys_addr_t)curr_cpu_info->msg_output,
                 _4KB_,
                 VMM_PAGE_NORMAL, MAP_UPDATE);
    map_memory (&curr_cpu_info->vmm_info.vmm_page_tables,
                 (phys_addr_t)curr_cpu_info->msg_ready, (phys_addr_t)curr_cpu_info->msg_ready + _4KB_,
                 (phys_addr_t)curr_cpu_info->msg_ready,
                 _4KB_,
                 VMM_PAGE_NORMAL, MAP_UPDATE);

    map_memory (&curr_cpu_info->vmm_info.vmm_page_tables,
                 (phys_addr_t)curr_cpu_info, (phys_addr_t)curr_cpu_info + _4KB_,
                 (phys_addr_t)curr_cpu_info,
                 _4KB_,
                 VMM_PAGE_NORMAL, MAP_UPDATE);

    map_memory (&curr_cpu_info->vmm_info.vmm_page_tables,
                 curr_cpu_info->vmm_info.vmm_start_vaddr, curr_cpu_info->vmm_info.vmm_end_vaddr,
                 curr_cpu_info->vmm_info.vmm_start_paddr,
                 _4KB_,
                 VMM_PAGE_NORMAL, MAP_UPDATE);

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
    get_cpu_info (i)->vm_info.vm_start_vaddr = 0;
    get_cpu_info (i)->vm_info.vm_start_paddr = 0;
  }
#undef VIRT2PHYS
}
static void
load_user_vms (phys_addr_t *vms_array)
{
  size_t vm_size;     /* VMM's Code + data + rodata + bss + stack */
  int ph_num;          /* VMM's number of program headers */
  int i;
  cpuid_t curr_cpu;
  struct cpu_info *curr_cpu_info;
  Elf64_Ehdr *elf_hdr;  /* Start address of executable */
  Elf64_Phdr *s;

  for (curr_cpu = NUMBER_SERVERS; curr_cpu < NUMBER_SERVERS + NUMBER_USER_VMS; curr_cpu++) {
    curr_cpu_info = get_cpu_info (curr_cpu);

    curr_cpu_info->vmm_info.vmm_has_vm = true;   /* This VMM, has a VM to run */
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

      section_src_paddr = vms_array[curr_cpu] + s->p_offset;  /* Address of section in executable */
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
      panic ("Second Boot Stage: Computed two different VM end addresses %x & %x\n", curr_cpu_info->vm_info.vm_end_paddr, curr_cpu_info->vm_info.vm_start_paddr + vm_size);
    }

#if 0
    cprintk ("code p = %x v = %x\ndata p = %x v = %x\nrodata p = %x v = %x\nbss p = %x v = %x\nstack p = %x v = %x\n", 0xC, curr_cpu_info->vm_info.vm_code_paddr, curr_cpu_info->vm_info.vm_code_vaddr,
        curr_cpu_info->vm_info.vm_data_paddr, curr_cpu_info->vm_info.vm_data_vaddr,
        curr_cpu_info->vm_info.vm_rodata_paddr, curr_cpu_info->vm_info.vm_rodata_vaddr,
        curr_cpu_info->vm_info.vm_bss_paddr, curr_cpu_info->vm_info.vm_bss_vaddr,
        curr_cpu_info->vm_info.vm_stack_paddr, curr_cpu_info->vm_info.vm_stack_vaddr);
    cprintk ("========================\n", 0xA);
#endif
  } /* For */

}
static void
load_server_vms (phys_addr_t *vms_array)
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
    curr_vm_phys_addr = (phys_addr_t)malloc_align (VM_MAX_SIZE, VM_MAX_SIZE);
    if (curr_vm_phys_addr == 0) {
      panic ("Second Boot Stage: Memory Allocation Failed! Line %d\n", __LINE__);
    }

    curr_cpu_info = get_cpu_info (curr_cpu);

    curr_cpu_info->vmm_info.vmm_has_vm = true;  /* This VMM, has a VM to run */

    curr_cpu_info->vm_info.vm_start_paddr = curr_vm_phys_addr;
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
      panic ("Unexpected VMM Header!\n");
    }

    s = (Elf64_Phdr*)((uint8_t*)vms_array[curr_cpu] + elf_hdr->e_phoff);
    /*
     * For servers, virtual & physical addresses are the same
     */
    curr_cpu_info->vm_info.vm_start_vaddr = (virt_addr_t)s->p_vaddr;
    curr_cpu_info->vm_info.vm_start_paddr = (phys_addr_t)s->p_vaddr;

    /* Why Upper bound is ph_num - 1?
     * Because in the VMM ELF executable, last section is .bss which does not have any byes.
     * So we don't need to copy any bytes from this section into memory. But we do need to allocate
     * enough memory for this section when we load the executable, and we
     * also need to zero out allocated memory. That's why we put .bss section at the
     * end of executable.
     */
    for (i = 0; i < ph_num - 1; i++) {
      phys_addr_t section_src_paddr = 0;
      phys_addr_t section_dst_paddr = 0;
      size_t section_size;

      section_src_paddr = vms_array[curr_cpu] + s->p_offset;  /* Address of section in executable */
      section_size = (size_t)s->p_memsz;               /* Section size */
      if (section_size > 0) {
        /* Address of section when loaded in ram
         * For Servers, virtual & physical addresses are the same
         */
        section_dst_paddr = (phys_addr_t)s->p_vaddr;
      }

      switch (i) {
        case 0:  /* Code */
          if (section_size == 0) {
            panic ("VM without code section!");
          }
          if (curr_vm_phys_addr != s->p_vaddr) {
            panic ("VM Server number %d can't be loaded in the right address %x. Wrong address is %x\n", curr_cpu, s->p_vaddr, curr_vm_phys_addr);
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
    curr_cpu_info->vm_info.vm_bss_paddr = s->p_vaddr;

    if (s->p_memsz > 0) {
      curr_cpu_info->vm_info.vm_bss_size = s->p_memsz;
      memset ((void*)curr_cpu_info->vm_info.vm_bss_paddr, 0, s->p_memsz);
    }

    memset ((void*)s->p_vaddr, 0, s->p_memsz);
    /*
     * Here we want to reserve some space for the stack. Stack should be
     * 16 byte aligned!
     */
    curr_cpu_info->vm_info.vm_stack_vaddr = ALIGN ((virt_addr_t)s->p_vaddr + s->p_memsz, 16);
    curr_cpu_info->vm_info.vm_stack_paddr = curr_cpu_info->vm_info.vm_stack_vaddr;
    curr_cpu_info->vm_info.vm_stack_size  = VM_STACK_SIZE;
    curr_cpu_info->vm_info.vm_regs.rsp = curr_cpu_info->vm_info.vm_stack_vaddr + curr_cpu_info->vm_info.vm_stack_size;

    vm_size = curr_cpu_info->vm_info.vm_stack_vaddr - curr_cpu_info->vm_info.vm_start_vaddr;

    curr_cpu_info->vm_info.vm_end_vaddr = curr_cpu_info->vm_info.vm_start_vaddr + vm_size;
    curr_cpu_info->vm_info.vm_end_paddr = curr_cpu_info->vm_info.vm_start_paddr + vm_size;

    if (curr_cpu_info->vm_info.vm_end_paddr != curr_cpu_info->vm_info.vm_end_vaddr) {
      panic ("Second Boot Stage: VM is worngfully loaded into memory!");
    }

    if (vm_size > VM_MAX_SIZE) {
      panic ("Second Boot Stage: vm is too large!");
    }


    /* Message Box */
    curr_cpu_info->msg_input  = get_msg_input(curr_cpu);
    curr_cpu_info->msg_output = get_msg_output(curr_cpu);
    curr_cpu_info->msg_ready  = get_msg_ready(curr_cpu);
#if 0
    cprintk ("SERVER: code p = %x v = %x size = %d\ndata p = %x v = %x size = %d\nrodata p = %x v = %x size = %d\nbss p = %x v = %x size = %d\nstack p = %x v = %x size = %d\n end = %x\n", 0xC,
        curr_cpu_info->vm_info.vm_code_paddr, curr_cpu_info->vm_info.vm_code_vaddr, curr_cpu_info->vm_info.vm_code_size,
        curr_cpu_info->vm_info.vm_data_paddr, curr_cpu_info->vm_info.vm_data_vaddr, curr_cpu_info->vm_info.vm_data_size,
        curr_cpu_info->vm_info.vm_rodata_paddr, curr_cpu_info->vm_info.vm_rodata_vaddr, curr_cpu_info->vm_info.vm_rodata_size,
        curr_cpu_info->vm_info.vm_bss_paddr, curr_cpu_info->vm_info.vm_bss_vaddr, curr_cpu_info->vm_info.vm_bss_size,
        curr_cpu_info->vm_info.vm_stack_paddr, curr_cpu_info->vm_info.vm_stack_vaddr, curr_cpu_info->vm_info.vm_stack_size,
        curr_cpu_info->vm_info.vm_end_vaddr);
    cprintk ("========================\n", 0xA);
#endif
  } /* For */

  for (curr_cpu = 0; curr_cpu < get_ncpus (); curr_cpu++) {
    curr_cpu_info = get_cpu_info (curr_cpu);
  }
}
/* ========================================== */
static void
EPT_map_memory_user (struct cpu_info *curr_cpu_info)
{
  map_memory (&curr_cpu_info->vm_info.vm_page_tables,
      0, (virt_addr_t)((virt_addr_t)_1GB_ * 4),
      0,
      _1GB_,
      VM_PAGE_NORMAL, MAP_NEW);

  /* TODO:
   *      Check carefully whether all these parts of memory are needed to
   *      be mapped for all the VMs.
   */
  EPT_map_memory (&curr_cpu_info->vm_info.vm_ept,
      0, _1MB_,
      0,
      USER_VMS_PAGE_SIZE,
      EPT_PAGE_READ | EPT_PAGE_WRITE, MAP_NEW);  /* XXX: Why do we need write access here? */
  /*
   * XXX:
   *    This may cause a bug. Because We are mapping more than needed here.
   *    We need to map only 8KB of memory, but we are actually mapping 2MB
   */
  EPT_map_memory (&curr_cpu_info->vm_info.vm_ept,
      curr_cpu_info->vm_info.vm_page_tables, curr_cpu_info->vm_info.vm_page_tables + 2 * _4KB_,
      curr_cpu_info->vm_info.vm_page_tables,
      USER_VMS_PAGE_SIZE,
      EPT_PAGE_READ, MAP_UPDATE);

  EPT_map_memory (&curr_cpu_info->vm_info.vm_ept,
      (phys_addr_t)curr_cpu_info, (phys_addr_t)curr_cpu_info + _4KB_,
      (phys_addr_t)curr_cpu_info,
      USER_VMS_PAGE_SIZE,
      EPT_PAGE_READ, MAP_UPDATE);
  EPT_map_memory (&curr_cpu_info->vm_info.vm_ept,
      (phys_addr_t)curr_cpu_info->msg_input, (phys_addr_t)curr_cpu_info->msg_input + _4KB_,
      (phys_addr_t)curr_cpu_info->msg_input,
      USER_VMS_PAGE_SIZE,
      EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);
  EPT_map_memory (&curr_cpu_info->vm_info.vm_ept,
      (phys_addr_t)curr_cpu_info->msg_output, (phys_addr_t)curr_cpu_info->msg_output + _4KB_,
      (phys_addr_t)curr_cpu_info->msg_output,
      USER_VMS_PAGE_SIZE,
      EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);
  EPT_map_memory (&curr_cpu_info->vm_info.vm_ept,
      (phys_addr_t)curr_cpu_info->msg_ready, (phys_addr_t)curr_cpu_info->msg_ready + _4KB_,
      (phys_addr_t)curr_cpu_info->msg_ready,
      USER_VMS_PAGE_SIZE,
      EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);
  if (curr_cpu_info->vm_info.vm_code_size > 0) {
    EPT_map_memory (&curr_cpu_info->vm_info.vm_ept,
        curr_cpu_info->vm_info.vm_code_vaddr, curr_cpu_info->vm_info.vm_code_vaddr + curr_cpu_info->vm_info.vm_code_size,
        curr_cpu_info->vm_info.vm_code_paddr,
        USER_VMS_PAGE_SIZE,
        EPT_PAGE_READ | EPT_PAGE_EXEC, MAP_UPDATE);
  }
  if (curr_cpu_info->vm_info.vm_data_size > 0) {
    EPT_map_memory (&curr_cpu_info->vm_info.vm_ept,
        curr_cpu_info->vm_info.vm_data_vaddr, curr_cpu_info->vm_info.vm_data_vaddr + curr_cpu_info->vm_info.vm_data_size,
        curr_cpu_info->vm_info.vm_data_paddr,
        USER_VMS_PAGE_SIZE,
        EPT_PAGE_READ | EPT_PAGE_EXEC, MAP_UPDATE);
  }
  if (curr_cpu_info->vm_info.vm_rodata_size > 0) {
    EPT_map_memory (&curr_cpu_info->vm_info.vm_ept,
        curr_cpu_info->vm_info.vm_rodata_vaddr, curr_cpu_info->vm_info.vm_rodata_vaddr + curr_cpu_info->vm_info.vm_rodata_size,
        curr_cpu_info->vm_info.vm_rodata_paddr,
        USER_VMS_PAGE_SIZE,
        EPT_PAGE_READ, MAP_UPDATE);
  }
  if (curr_cpu_info->vm_info.vm_bss_size > 0) {
    EPT_map_memory (&curr_cpu_info->vm_info.vm_ept,
        curr_cpu_info->vm_info.vm_bss_vaddr, curr_cpu_info->vm_info.vm_bss_vaddr + curr_cpu_info->vm_info.vm_bss_size,
        curr_cpu_info->vm_info.vm_bss_paddr,
        USER_VMS_PAGE_SIZE,
        EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);
  }
  if (curr_cpu_info->vm_info.vm_stack_size > 0) {
    EPT_map_memory (&curr_cpu_info->vm_info.vm_ept,
        curr_cpu_info->vm_info.vm_stack_vaddr, curr_cpu_info->vm_info.vm_stack_vaddr + curr_cpu_info->vm_info.vm_stack_size,
        curr_cpu_info->vm_info.vm_stack_paddr,
        USER_VMS_PAGE_SIZE,
        EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);
  }
}
/* ========================================== */
static void
EPT_map_memory_server (struct cpu_info *curr_cpu_info)
{
  map_memory (&curr_cpu_info->vm_info.vm_page_tables,
      0, (virt_addr_t)((virt_addr_t)_1GB_ * 4),
      0,
      _1GB_,
      VM_PAGE_NORMAL, MAP_NEW);
  EPT_map_memory (&curr_cpu_info->vm_info.vm_ept,
      0, (virt_addr_t)((virt_addr_t)_1GB_ * 4),
      0,
      _1GB_,
      EPT_PAGE_READ | EPT_PAGE_WRITE | EPT_PAGE_EXEC, MAP_NEW);
}
/* ========================================== */
static void
load_all_vms (phys_addr_t *vms_array, phys_addr_t boot_stage2_end_addr)
{
  cpuid_t curr_cpu;

  load_server_vms (vms_array);
  load_user_vms (vms_array);

  for (curr_cpu = NUMBER_SERVERS; curr_cpu < NUMBER_SERVERS + NUMBER_USER_VMS; curr_cpu++) {
    EPT_map_memory_user (get_cpu_info (curr_cpu));
  }
  /* ============================================ *
   * Mapping Servers
   * ============================================ */
  for (curr_cpu = 0; curr_cpu < NUMBER_SERVERS; curr_cpu++) {
    EPT_map_memory_server (get_cpu_info (curr_cpu));

  }
}
static void
set_vm_args (struct boot_stage2_args *boot_args)
{
  static struct pm_args pm_arguments;

  pm_arguments.memory_size = get_mem_size ();
  pm_arguments.last_used_addr = get_last_used_addr ();
  get_cpu_info (PM)->vm_args = &pm_arguments;

  get_cpu_info(FS)->vm_args = (void *)boot_args->initrd_elf_addr;
}
/* ========================================== */
void
boot_stage2_main (struct boot_stage2_args *boot_args)
{
  phys_addr_t vms_array[] = {boot_args->pm_elf_addr, boot_args->fs_elf_addr, boot_args->init_elf_addr};

  con_init ();
  memory_init (boot_args->boot_stage2_end_addr, boot_args->sys_mem_size);

  map_memory (&(boot_args->boot_stage2_page_tables), 0xFEC00000, 0x100000000, 0xFEC00000, _2MB_, VMM_PAGE_UNCACHABLE, MAP_UPDATE);

  mp_init ();

  msg_input = (struct message *)calloc_align (sizeof (uint8_t), get_ncpus() * _4KB_, _2MB_);
  msg_output = (struct message *)calloc_align (sizeof (uint8_t), get_ncpus() * _4KB_, _2MB_);
  msg_ready = (bool *)calloc_align (sizeof (uint8_t), get_ncpus() * _4KB_, _2MB_);

  interrupt_init ();

  pic_init ();
  ioapic_init ();
  lapic_init();
  kbd_init ();
  load_all_vmms (boot_args->vmm_elf_addr, boot_args->boot_stage2_end_addr);
  load_all_vms (vms_array, boot_args->boot_stage2_end_addr);
  /* NOTE:
   *  Since we are setting the PM->last_used_addr, no one after this function
   *  call should allocate any more memory before PM is booted.
   */
  set_vm_args (boot_args);

  mp_bootothers ();
  __asm__ __volatile__ ("sti\n");

  for (;;);
}

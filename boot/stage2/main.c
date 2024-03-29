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
#include <asmfunc.h>
#include <cpuinfo.h>
#include <vuos/vuos.h>
/* ========================================== */
static struct message *msg_input;
static struct message *msg_output;
static bool *msg_ready;
/* ========================================== */
static bool has_1GB_page;
/* ========================================== */
static __inline struct message *
get_msg_input (cpuid_t cpuid)
{
  if (cpuid > get_ncpus ()) {
    panic ("get_vmxon_ptr: cpuid out of rainge!");
  }

  return (struct message *)((phys_addr_t)msg_input + cpuid * _4KB_);
}
/* ========================================== */
static __inline struct message *
get_msg_output (cpuid_t cpuid)
{
  if (cpuid > get_ncpus ()) {
    panic ("get_vmxon_ptr: cpuid out of rainge!");
  }

  return (struct message *)((phys_addr_t)msg_output + cpuid * _4KB_);
}
/* ========================================== */
static __inline bool *
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

  } /* For */

  for (curr_cpu = 0; curr_cpu < get_ncpus (); curr_cpu++) {
    curr_cpu_info = get_cpu_info (curr_cpu);

    //cprintk ("Last MAPED ADDRESS = %x\n", 0xF, (phys_addr_t)get_cpu_info (get_ncpus () - 1) + sizeof (struct cpu_info));
    map_memory (&curr_cpu_info->vmm_info.vmm_page_tables,
                 0, boot_stage2_end_addr,
                 0,
                 _4KB_,
                 PAGE_PRESENT | PAGE_RW | PAGE_PCD, MAP_NEW);  /* PAGE_PCD because of VGA */
    //cprintk ("cpuinfo_table = %x curr_cpu_info %x cpuid - %d\n", 0x2, cpuinfo_table, curr_cpu_info, curr_cpu_info->cpuid);
    map_memory (&curr_cpu_info->vmm_info.vmm_page_tables,
                 curr_cpu_info->vm_info.vm_vmxon_ptr, curr_cpu_info->vm_info.vm_vmxon_ptr + _4KB_,
                 curr_cpu_info->vm_info.vm_vmxon_ptr,
                 _4KB_,
                 PAGE_PRESENT | PAGE_RW, MAP_UPDATE);
    map_memory (&curr_cpu_info->vmm_info.vmm_page_tables,
                 curr_cpu_info->vm_info.vm_vmcs_ptr, curr_cpu_info->vm_info.vm_vmcs_ptr + _4KB_,
                 curr_cpu_info->vm_info.vm_vmcs_ptr,
                 _4KB_,
                 PAGE_PRESENT | PAGE_RW, MAP_UPDATE);
    map_memory (&curr_cpu_info->vmm_info.vmm_page_tables,
                 (phys_addr_t)curr_cpu_info->msg_input, (phys_addr_t)curr_cpu_info->msg_input + _4KB_,
                 (phys_addr_t)curr_cpu_info->msg_input,
                 _4KB_,
                 PAGE_PRESENT | PAGE_RW, MAP_UPDATE);
    map_memory (&curr_cpu_info->vmm_info.vmm_page_tables,
                 (phys_addr_t)curr_cpu_info->msg_output, (phys_addr_t)curr_cpu_info->msg_output + _4KB_,
                 (phys_addr_t)curr_cpu_info->msg_output,
                 _4KB_,
                 PAGE_PRESENT | PAGE_RW, MAP_UPDATE);
    map_memory (&curr_cpu_info->vmm_info.vmm_page_tables,
                 (phys_addr_t)curr_cpu_info->msg_ready, (phys_addr_t)curr_cpu_info->msg_ready + _4KB_,
                 (phys_addr_t)curr_cpu_info->msg_ready,
                 _4KB_,
                 PAGE_PRESENT | PAGE_RW, MAP_UPDATE);

    map_memory (&curr_cpu_info->vmm_info.vmm_page_tables,
                 (phys_addr_t)curr_cpu_info, (phys_addr_t)curr_cpu_info + _4KB_,
                 (phys_addr_t)curr_cpu_info,
                 _4KB_,
                 PAGE_PRESENT | PAGE_RW, MAP_UPDATE);

    map_memory (&curr_cpu_info->vmm_info.vmm_page_tables,
                 curr_cpu_info->vmm_info.vmm_start_vaddr, curr_cpu_info->vmm_info.vmm_end_vaddr,
                 curr_cpu_info->vmm_info.vmm_start_paddr,
                 _4KB_,
                 PAGE_PRESENT | PAGE_RW, MAP_UPDATE);
    map_memory (&curr_cpu_info->vmm_info.vmm_page_tables,
                ((virt_addr_t)0xFEC00000), (virt_addr_t)((virt_addr_t)_1GB_ * 4),
                ((virt_addr_t)0xFEC00000),
                _4KB_,
                PAGE_PRESENT | PAGE_RW | PAGE_PCD, MAP_UPDATE);
  }
#undef VIRT2PHYS
}
static void
load_other_vms (cpuid_t cpuid, phys_addr_t vm_elf_paddr)
{
  size_t vm_size;     /* VMM's Code + data + rodata + bss + stack */
  int ph_num;          /* VMM's number of program headers */
  int i;
  struct cpu_info *curr_cpu_info;
  Elf64_Ehdr *elf_hdr;  /* Start address of executable */
  Elf64_Phdr *s;

    curr_cpu_info = get_cpu_info (cpuid);

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
    elf_hdr = (Elf64_Ehdr*)vm_elf_paddr;

    ph_num = elf_hdr->e_phnum;    /* Number of program headers in ELF executable */
    if (ph_num != 4) {
      cprintk ("ELF executable contains %d sections!\n", 0x4, ph_num);
      panic ("Unexpected VM Header!\n");
    }

    s = (Elf64_Phdr*)((uint8_t*)vm_elf_paddr + elf_hdr->e_phoff);
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

      section_src_paddr = vm_elf_paddr + s->p_offset;  /* Address of section in executable */
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

    curr_cpu_info->vm_info.vm_regs.rflags = 0x2;  /* No Flags set!!! */

    vm_size = (curr_cpu_info->vm_info.vm_stack_paddr + curr_cpu_info->vm_info.vm_stack_size) - curr_cpu_info->vm_info.vm_start_paddr;

    curr_cpu_info->vm_info.vm_end_vaddr = curr_cpu_info->vm_info.vm_start_vaddr + vm_size;
    curr_cpu_info->vm_info.vm_end_paddr = curr_cpu_info->vm_info.vm_start_paddr + vm_size;

    if (curr_cpu_info->vm_info.vm_end_paddr != curr_cpu_info->vm_info.vm_start_paddr + vm_size) {
      panic ("Second Boot Stage: Computed two different VM end addresses %x & %x\n", curr_cpu_info->vm_info.vm_end_paddr, curr_cpu_info->vm_info.vm_start_paddr + vm_size);
    }

    /* Set the FDs pointer to NULL at the beginning */
    curr_cpu_info->vm_info.vm_fds = NULL;
#if 0
    cprintk ("code p = %x v = %x\ndata p = %x v = %x\nrodata p = %x v = %x\nbss p = %x v = %x\nstack p = %x v = %x\n", 0xC, curr_cpu_info->vm_info.vm_code_paddr, curr_cpu_info->vm_info.vm_code_vaddr,
        curr_cpu_info->vm_info.vm_data_paddr, curr_cpu_info->vm_info.vm_data_vaddr,
        curr_cpu_info->vm_info.vm_rodata_paddr, curr_cpu_info->vm_info.vm_rodata_vaddr,
        curr_cpu_info->vm_info.vm_bss_paddr, curr_cpu_info->vm_info.vm_bss_vaddr,
        curr_cpu_info->vm_info.vm_stack_paddr, curr_cpu_info->vm_info.vm_stack_vaddr);
    cprintk ("========================\n", 0xA);
#endif
}
static void
load_server_vms (cpuid_t cpuid, phys_addr_t vm_elf_paddr)
{
  Elf64_Ehdr *elf_hdr;  /* Start address of executable */
  Elf64_Phdr *s;

  phys_addr_t curr_vm_phys_addr;

  size_t vm_size;     /* VMM's Code + data + rodata + bss + stack */
  int ph_num;          /* VMM's number of program headers */
  int i;
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
  curr_vm_phys_addr = (phys_addr_t)malloc_align (VM_MAX_SIZE, VM_MAX_SIZE);
  if (curr_vm_phys_addr == 0) {
    panic ("Second Boot Stage: Memory Allocation Failed! Line %d\n", __LINE__);
  }

  curr_cpu_info = get_cpu_info (cpuid);

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
  elf_hdr = (Elf64_Ehdr*)vm_elf_paddr;  /* Start address of executable */

  ph_num = elf_hdr->e_phnum;    /* Number of program headers in ELF executable */
  if (ph_num != 4) {
    cprintk ("ELF executable contains %d sections!\n", 0x4, ph_num);
    panic ("Unexpected VMM Header!\n");
  }

  s = (Elf64_Phdr*)((uint8_t*)vm_elf_paddr + elf_hdr->e_phoff);
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

    section_src_paddr = vm_elf_paddr + s->p_offset;  /* Address of section in executable */
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
          panic ("VM Server number %d can't be loaded in the right address %x. Wrong address is %x\n", cpuid, s->p_vaddr, curr_vm_phys_addr);
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

  curr_cpu_info->vm_info.vm_regs.rflags = 0x2;  /* No Flags set!!! */

  vm_size = curr_cpu_info->vm_info.vm_stack_vaddr - curr_cpu_info->vm_info.vm_start_vaddr;

  curr_cpu_info->vm_info.vm_end_vaddr = curr_cpu_info->vm_info.vm_start_vaddr + vm_size;
  curr_cpu_info->vm_info.vm_end_paddr = curr_cpu_info->vm_info.vm_start_paddr + vm_size;

  if (curr_cpu_info->vm_info.vm_end_paddr != curr_cpu_info->vm_info.vm_end_vaddr) {
    panic ("Second Boot Stage: VM is worngfully loaded into memory!");
  }

  if (vm_size > VM_MAX_SIZE) {
    panic ("Second Boot Stage: vm is too large!");
  }

  /* Set the FDs pointer to NULL at the beginning */
  curr_cpu_info->vm_info.vm_fds = NULL;


#if 0
  /* Message Box */
  /* These are not needed since are already set in load_all_vmms */
  curr_cpu_info->msg_input  = get_msg_input(curr_cpu);
  curr_cpu_info->msg_output = get_msg_output(curr_cpu);
  curr_cpu_info->msg_ready  = get_msg_ready(curr_cpu);
  cprintk ("SERVER: code p = %x v = %x size = %d\ndata p = %x v = %x size = %d\nrodata p = %x v = %x size = %d\nbss p = %x v = %x size = %d\nstack p = %x v = %x size = %d\n end = %x\n", 0xC,
      curr_cpu_info->vm_info.vm_code_paddr, curr_cpu_info->vm_info.vm_code_vaddr, curr_cpu_info->vm_info.vm_code_size,
      curr_cpu_info->vm_info.vm_data_paddr, curr_cpu_info->vm_info.vm_data_vaddr, curr_cpu_info->vm_info.vm_data_size,
      curr_cpu_info->vm_info.vm_rodata_paddr, curr_cpu_info->vm_info.vm_rodata_vaddr, curr_cpu_info->vm_info.vm_rodata_size,
      curr_cpu_info->vm_info.vm_bss_paddr, curr_cpu_info->vm_info.vm_bss_vaddr, curr_cpu_info->vm_info.vm_bss_size,
      curr_cpu_info->vm_info.vm_stack_paddr, curr_cpu_info->vm_info.vm_stack_vaddr, curr_cpu_info->vm_info.vm_stack_size,
      curr_cpu_info->vm_info.vm_end_vaddr);
  cprintk ("========================\n", 0xA);
#endif
}
/* ========================================== */
static void
ept_map_memory_other (struct cpu_info *curr_cpu_info)
{
  map_memory (&curr_cpu_info->vm_info.vm_page_tables,
      0, (virt_addr_t)((virt_addr_t)_2MB_),
      0,
      has_1GB_page ? _1GB_ : _2MB_,
      PAGE_PRESENT | PAGE_RW | PAGE_PCD, MAP_NEW);  /* PAGE_PCD because of VGA */
  map_memory (&curr_cpu_info->vm_info.vm_page_tables,
      _2MB_, ((virt_addr_t)0xFEC00000),
      _2MB_,
      has_1GB_page ? _1GB_ : _2MB_,
      PAGE_PRESENT | PAGE_RW, MAP_UPDATE);
  map_memory (&curr_cpu_info->vm_info.vm_page_tables,
      ((virt_addr_t)0xFEC00000), (virt_addr_t)((virt_addr_t)_1GB_ * 4),
      ((virt_addr_t)0xFEC00000),
      has_1GB_page ? _1GB_ : _2MB_,
      PAGE_PRESENT | PAGE_RW | PAGE_PCD, MAP_UPDATE);
#if 0
  map_memory (&curr_cpu_info->vm_info.vm_page_tables,
     (virt_addr_t)((virt_addr_t)_1GB_ * 1), (virt_addr_t)((virt_addr_t)_1GB_ * 4),
      (virt_addr_t)((virt_addr_t)_1GB_ * 1),
      has_1GB_page ? _1GB_ : _2MB_,
      PAGE_PRESENT | PAGE_RW | PAGE_PCD, MAP_UPDATE);
#endif

  /* TODO:
   *      Check carefully whether all these parts of memory are needed to
   *      be mapped for all the VMs.
   */
  ept_map_memory (&curr_cpu_info->vm_info.vm_ept,
      0xB8000, 0xB9000,  /* Just for debugging purposes */
      0xB8000,
      USER_VMS_PAGE_SIZE,
      EPT_MTYPE_UC,
      EPT_PAGE_READ | EPT_PAGE_WRITE, MAP_NEW);  /* XXX: Why do we need write access here? */
  /*
   * XXX:
   *    This may cause a bug. Because We are mapping more than needed here.
   *    We need to map only 8KB of memory, but we are actually mapping 2MB
   */
  ept_map_page_tables (&curr_cpu_info->vm_info.vm_ept, curr_cpu_info->vm_info.vm_page_tables,
      EPT_PAGE_READ | EPT_PAGE_WRITE);

  ept_map_memory (&curr_cpu_info->vm_info.vm_ept,
      (phys_addr_t)curr_cpu_info, (phys_addr_t)curr_cpu_info + _4KB_,
      (phys_addr_t)curr_cpu_info,
      USER_VMS_PAGE_SIZE,
      EPT_MTYPE_WB,
      EPT_PAGE_READ, MAP_UPDATE);
  ept_map_memory (&curr_cpu_info->vm_info.vm_ept,
      (phys_addr_t)curr_cpu_info->msg_input, (phys_addr_t)curr_cpu_info->msg_input + _4KB_,
      (phys_addr_t)curr_cpu_info->msg_input,
      USER_VMS_PAGE_SIZE,
      EPT_MTYPE_WB,
      EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);
  ept_map_memory (&curr_cpu_info->vm_info.vm_ept,
      (phys_addr_t)curr_cpu_info->msg_output, (phys_addr_t)curr_cpu_info->msg_output + _4KB_,
      (phys_addr_t)curr_cpu_info->msg_output,
      USER_VMS_PAGE_SIZE,
      EPT_MTYPE_WB,
      EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);
  ept_map_memory (&curr_cpu_info->vm_info.vm_ept,
      (phys_addr_t)curr_cpu_info->msg_ready, (phys_addr_t)curr_cpu_info->msg_ready + _4KB_,
      (phys_addr_t)curr_cpu_info->msg_ready,
      USER_VMS_PAGE_SIZE,
      EPT_MTYPE_WB,
      EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);
  ept_map_memory (&curr_cpu_info->vm_info.vm_ept,
      (phys_addr_t)curr_cpu_info->msg_ready_bitmap, (phys_addr_t)curr_cpu_info->msg_ready_bitmap + _4KB_,
      (phys_addr_t)curr_cpu_info->msg_ready_bitmap,
      USER_VMS_PAGE_SIZE,
      EPT_MTYPE_WB,  /* If we are going to use "monitor/mwait", mem type must be write-back */
      EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);
  if (curr_cpu_info->vm_info.vm_code_size > 0) {
    ept_map_memory (&curr_cpu_info->vm_info.vm_ept,
        curr_cpu_info->vm_info.vm_code_vaddr, curr_cpu_info->vm_info.vm_code_vaddr + curr_cpu_info->vm_info.vm_code_size,
        curr_cpu_info->vm_info.vm_code_paddr,
        USER_VMS_PAGE_SIZE,
        EPT_MTYPE_WB,
        EPT_PAGE_READ | EPT_PAGE_EXEC, MAP_UPDATE);
  }
  if (curr_cpu_info->vm_info.vm_data_size > 0) {
    ept_map_memory (&curr_cpu_info->vm_info.vm_ept,
        curr_cpu_info->vm_info.vm_data_vaddr, curr_cpu_info->vm_info.vm_data_vaddr + curr_cpu_info->vm_info.vm_data_size,
        curr_cpu_info->vm_info.vm_data_paddr,
        USER_VMS_PAGE_SIZE,
        EPT_MTYPE_WB,
        EPT_PAGE_READ | EPT_PAGE_EXEC, MAP_UPDATE);
  }
  if (curr_cpu_info->vm_info.vm_rodata_size > 0) {
    ept_map_memory (&curr_cpu_info->vm_info.vm_ept,
        curr_cpu_info->vm_info.vm_rodata_vaddr, curr_cpu_info->vm_info.vm_rodata_vaddr + curr_cpu_info->vm_info.vm_rodata_size,
        curr_cpu_info->vm_info.vm_rodata_paddr,
        USER_VMS_PAGE_SIZE,
        EPT_MTYPE_WB,
        EPT_PAGE_READ, MAP_UPDATE);
  }
  if (curr_cpu_info->vm_info.vm_bss_size > 0) {
    ept_map_memory (&curr_cpu_info->vm_info.vm_ept,
        curr_cpu_info->vm_info.vm_bss_vaddr, curr_cpu_info->vm_info.vm_bss_vaddr + curr_cpu_info->vm_info.vm_bss_size,
        curr_cpu_info->vm_info.vm_bss_paddr,
        USER_VMS_PAGE_SIZE,
        EPT_MTYPE_WB,
        EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);
  }
  if (curr_cpu_info->vm_info.vm_stack_size > 0) {
    ept_map_memory (&curr_cpu_info->vm_info.vm_ept,
        curr_cpu_info->vm_info.vm_stack_vaddr, curr_cpu_info->vm_info.vm_stack_vaddr + curr_cpu_info->vm_info.vm_stack_size,
        curr_cpu_info->vm_info.vm_stack_paddr,
        USER_VMS_PAGE_SIZE,
        EPT_MTYPE_WB,
        EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);
  }
  ept_map_memory (&curr_cpu_info->vm_info.vm_ept,
      ((virt_addr_t)0xFEE00000), (virt_addr_t)((virt_addr_t)_1GB_ * 4),
      ((virt_addr_t)0xFEE00000),
      USER_VMS_PAGE_SIZE,
      EPT_MTYPE_UC,
      EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);
}
/* ========================================== */
static void
ept_map_memory_server (struct cpu_info *curr_cpu_info)
{
  map_memory (&curr_cpu_info->vm_info.vm_page_tables,
      0, (virt_addr_t)((virt_addr_t)_2MB_),
      0,
      has_1GB_page ? _1GB_ : _2MB_,
      PAGE_PRESENT | PAGE_RW | PAGE_PCD, MAP_NEW);
  map_memory (&curr_cpu_info->vm_info.vm_page_tables,
      _2MB_, ((virt_addr_t)0xFEC00000),
      _2MB_,
      has_1GB_page ? _1GB_ : _2MB_,
      PAGE_PRESENT | PAGE_RW, MAP_UPDATE);
  map_memory (&curr_cpu_info->vm_info.vm_page_tables,
      ((virt_addr_t)0xFEC00000), (virt_addr_t)((virt_addr_t)_1GB_ * 4),
      ((virt_addr_t)0xFEC00000),
      has_1GB_page ? _1GB_ : _2MB_,
      PAGE_PRESENT | PAGE_RW | PAGE_PCD, MAP_UPDATE);

  ept_map_memory (&curr_cpu_info->vm_info.vm_ept,
      0, (virt_addr_t)((virt_addr_t)_2MB_),
      0,
      has_1GB_page ? _1GB_ : _2MB_,
      EPT_MTYPE_UC,
      EPT_PAGE_READ | EPT_PAGE_WRITE, MAP_NEW);
  ept_map_memory (&curr_cpu_info->vm_info.vm_ept,
      _2MB_, ((virt_addr_t)0xFEC00000),
      _2MB_,
      has_1GB_page ? _1GB_ : _2MB_,
      EPT_MTYPE_WB,
      EPT_PAGE_READ | EPT_PAGE_WRITE | EPT_PAGE_EXEC, MAP_UPDATE);
  ept_map_memory (&curr_cpu_info->vm_info.vm_ept,
      ((virt_addr_t)0xFEC00000), (virt_addr_t)((virt_addr_t)_1GB_ * 4),
      ((virt_addr_t)0xFEC00000),
      has_1GB_page ? _1GB_ : _2MB_,
      EPT_MTYPE_UC,
      EPT_PAGE_WRITE | EPT_PAGE_READ, MAP_UPDATE);
}
/* ========================================== */
static void
load_all_vms (phys_addr_t *vms_array, phys_addr_t boot_stage2_end_addr)
{
  int i;
  struct cpu_info *curr_cpu_info;

  /* Order of loading is important. Don't change it! */
  load_server_vms (PM, vms_array[PM]);
  load_server_vms (FS, vms_array[FS]);
  load_other_vms (INIT, vms_array[INIT]);

  ept_map_memory_server (get_cpu_info (PM));
  ept_map_memory_server (get_cpu_info (FS));
  ept_map_memory_other  (get_cpu_info (INIT));

  /*
   * We save a pointer to cpu_info structure of each CPU
   * in rdi registers. This way we let VMs about their
   * cpu information.
   */
  for (i = 0; i < ALWAYS_BUSY; i++) {
    curr_cpu_info = get_cpu_info (i);
    curr_cpu_info->vm_info.vm_regs.rdi = (phys_addr_t)curr_cpu_info;
  }
#if 0
  cprintk ("PM: msg_ready %x msg_input %x msg_output = %x\n", 0xA, get_cpu_info (PM)->msg_ready, get_cpu_info (PM)->msg_input, get_cpu_info(PM)->msg_output);
  cprintk ("FS: msg_ready %x msg_input %x msg_output = %x\n", 0xA, get_cpu_info (FS)->msg_ready, get_cpu_info (FS)->msg_input, get_cpu_info(FS)->msg_output);
  halt ();
#endif
}
/* ========================================== */
static void
set_vm_args (struct boot_stage2_args *boot_args)
{
  static struct pm_args pm_arguments;

  pm_arguments.memory_size = get_mem_size ();
  pm_arguments.last_used_addr = get_last_used_addr ();
  pm_arguments.has_1GB_page = boot_args->has_1GB_page;
  get_cpu_info (PM)->vm_args = &pm_arguments;

  get_cpu_info(FS)->vm_args = (void *)boot_args->initrd_elf_addr;
}
/* ========================================== */
void
boot_stage2_main (struct boot_stage2_args *boot_args)
{
  int i;
  uint64_t cpu_freq;
  uint64_t bus_freq;

  phys_addr_t vms_elf_array[ALWAYS_BUSY];

  vms_elf_array[PM] = boot_args->pm_elf_addr;
  vms_elf_array[FS] = boot_args->fs_elf_addr;
  vms_elf_array[INIT] = boot_args->init_elf_addr;

  con_init ();

#if 0
  uint64_t cr0;
  uint64_t b, a;
  unsigned int aux;
  uint64_t sum = 0;
  cr0 = rcr0 ();
  lcr0(cr0 | (1 << 5));
  cr0 = rcr0 ();

  cprintk ("cr0 = %x\n\n", 0xB, cr0);
#define MAX 10000
  static volatile int hamid = 0;
  static volatile int francesco = 0;
  sum = 0;
  for (i = 0; i < MAX; i++) {
    b = rdtscp (&aux);
    hamid++;
    francesco = hamid;
    a = rdtscp (&aux);
    sum += (a - b);
  }
  cprintk ("written %d\n", 0xE, francesco);
  cprintk ("sum %d Took %d cycles\n", 0xC, sum, sum / MAX);
  halt ();
#endif
  memory_init (boot_args->boot_stage2_end_addr, boot_args->sys_mem_size);

  has_1GB_page = boot_args->has_1GB_page;

  map_memory (&(boot_args->boot_stage2_page_tables), 0xFEC00000, 0x100000000, 0xFEC00000, _2MB_, (PAGE_PRESENT | PAGE_RW | PAGE_PCD), MAP_UPDATE);

  mp_init ();

  msg_input = (struct message *)calloc_align (sizeof (uint8_t), get_ncpus() * _4KB_, _2MB_);
  msg_output = (struct message *)calloc_align (sizeof (uint8_t), get_ncpus() * _4KB_, _2MB_);
  msg_ready = (bool *)calloc_align (sizeof (uint8_t), get_ncpus() * _4KB_, _2MB_);

  uint64_t *bitmap = (uint64_t *)calloc_align (sizeof(uint64_t), get_ncpus(), _4KB_);

  cpu_freq = get_cpu_freq ();
  bus_freq = get_bus_freq ();

  for (i = 0; i < get_ncpus (); i++) {
    struct cpu_info *cpu;

    cpu = get_cpu_info (i);
    cpu->msg_input  = get_msg_input(i);
    cpu->msg_output = get_msg_output(i);
    /* XXX Msg_ready should not be needed anymore */
    cpu->msg_ready  = get_msg_ready(i);
    cpu->msg_ready_bitmap = bitmap;

    cpu->cpu_freq = cpu_freq;
    cpu->bus_freq = bus_freq;
  }

  interrupt_init ();

  pic_init ();
  ioapic_init ();
  lapic_init();
  //kbd_init ();  /* TODO: remove the coment, otherwise we will not have keyboard */
  /* TODO:
   * What the hell are we writing!!!! here. Remove the following line
   * and replace it with something like ioapic_enable_e1000() or something
   * like that.
   */
  //ioapic_enable (19, 3);
  /* ================================ */
  load_all_vms (vms_elf_array, boot_args->boot_stage2_end_addr);
  load_all_vmms (boot_args->vmm_elf_addr, boot_args->boot_stage2_end_addr);
  /* NOTE:
   *  Since we are setting the PM->last_used_addr, no one after this function
   *  call should allocate any more memory before PM is booted.
   */
  set_vm_args (boot_args);

  mp_bootothers ();
  __asm__ __volatile__ ("sti\n");

  for (;;);
}

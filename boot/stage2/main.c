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
/* ========================================== */
#define VMM_PAGE_UNCACHABLE (PAGE_PRESENT | PAGE_RW | PAGE_PSE | PAGE_PCD) /* Page is not cachable */
#define VMM_PAGE_NORMAL (PAGE_PRESENT | PAGE_RW | PAGE_PSE)
#define VM_PAGE_UNCACHABLE (PAGE_PRESENT | PAGE_RW | PAGE_PSE | PAGE_PCD) /* Page is not cachable */
#define VM_PAGE_NORMAL (PAGE_PRESENT | PAGE_RW | PAGE_PSE)
/* ========================================== */
#define ALIGN(addr, bound) (((addr)+((bound)-1))&(~((bound)-1)))
/* ========================================== */
#define _4KB_ 0x1000
#define _2MB_ (512 * _4KB_)
#define _1GB_ (512 * _2MB_)
/* ========================================== */
#define VMM_MAX_SIZE _2MB_
#define VM_MAX_SIZE  _2MB_
/* ========================================== */
#define MAP_NEW    0x0   /* Create New Page Tables */
#define MAP_UPDATE 0x1   /* Update existing Page Tables */
/* ========================================== */
phys_addr_t vmx_data_structure_pool;
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
void
map_memory (phys_addr_t *pml4_paddr,
            virt_addr_t memory_region_start_vaddr,
            virt_addr_t memory_region_end_vaddr,
            phys_addr_t memory_region_start_paddr,
            phys_addr_t *last_allocated_paddr,
            uint32_t page_size,
            uint16_t protection,
            int  flags)
{
  phys_addr_t paddr;
  virt_addr_t vaddr;
  page_table_entry_t *pml4 = NULL;
  page_table_entry_t *pdpe = NULL;
  page_table_entry_t *pde  = NULL;
  page_table_entry_t *pte  = NULL;
  int pml4_idx;
  int pdpe_idx;
  int pde_idx;
  int pte_idx;
  int page;
  int last_pages;
  size_t pages;
  size_t memory_region_size;

  *last_allocated_paddr = ALIGN ((*last_allocated_paddr), PAGE_TABLE_SIZE); 

  if (memory_region_start_vaddr > memory_region_end_vaddr) {
    panic ("mep_memory: memory start region can not be greater than end address");
  }

  if (memory_region_start_vaddr == memory_region_end_vaddr) {
    panic ("Can't map a memory region of zero size!");
  }

  if (flags == MAP_NEW) {
    pml4 = (page_table_entry_t *)(*last_allocated_paddr);
    
    *pml4_paddr = (phys_addr_t)pml4;

    *last_allocated_paddr += PAGE_TABLE_SIZE;

    memset (pml4, 0, PAGE_TABLE_SIZE);
  } else if (flags == MAP_UPDATE) {
    pml4 = (page_table_entry_t *)*pml4_paddr;

    if ((pml4[0] & (PAGE_PRESENT | PAGE_RW)) == 0) {
      cprintk ("Warning:\nSecond Boot Stage: Invalid PML4 used to map VMM Pages!", 0x4);
    }
    if (pml4[0] == 0) {
      panic ("Second Boot Stage: Trying to update null page tables!");
    }
  } else {
    panic ("Second Boot Stage: Undefined memory map flag!");
  }

  if (protection != VMM_PAGE_UNCACHABLE && protection != VMM_PAGE_NORMAL) {
    panic ("Second Boot Stage: Undefined VMM page protection!");
  }

  switch (page_size) {
    case _4KB_:
      break;
    case _2MB_:
    case _1GB_:
      protection |= PAGE_PSE;
      break;
    default:
      panic ("Second Boot Stage: Bad page size!");
  }

  memory_region_size =  memory_region_end_vaddr - memory_region_start_vaddr;

  pages = memory_region_size / page_size + (memory_region_size % page_size ? 1 : 0);

  paddr = memory_region_start_paddr;
  /*
   * In the whole stage2, virtual addresses are equal to
   * physical addresses
   */
  vaddr = memory_region_start_vaddr;
  /* TODO:
   *  1. Replace all constants here with some meaningfull macros.
   */ 
  page = 0;
  last_pages = pages;
  while (page != last_pages) {
    /* TODO:
     *     A boundry check to make sure none of indices go beyond
     *     512.
     */
    pml4_idx = (vaddr >> 39) & 0x1FF;
    pdpe_idx = (vaddr >> 30) & 0x1FF;
    pde_idx  = (vaddr >> 21) & 0x1FF;
    pte_idx  = (vaddr >> 12) & 0x1FF;


    if (pml4[pml4_idx] == 0) {
      pdpe = (page_table_entry_t *)(*last_allocated_paddr);

      *last_allocated_paddr += PAGE_TABLE_SIZE;

      memset (pdpe, 0, PAGE_TABLE_SIZE);

      pml4[pml4_idx] = (phys_addr_t)pdpe | (PAGE_PRESENT | PAGE_RW);
    }

    pdpe = (page_table_entry_t *)((phys_addr_t)pml4[pml4_idx] & ~0xFFF);
    if (pdpe[pdpe_idx] == 0 && page_size < _1GB_) {
      pde = (page_table_entry_t *)(*last_allocated_paddr);

      *last_allocated_paddr += PAGE_TABLE_SIZE;

      memset (pde, 0, PAGE_TABLE_SIZE);

      pdpe[pdpe_idx] = (phys_addr_t)pde | (PAGE_PRESENT | PAGE_RW);
    }
    if (page_size < _1GB_) {
      pde = (page_table_entry_t *)((phys_addr_t)pdpe[pdpe_idx] & ~0xFFF);

      if (pde[pde_idx] == 0 && page_size < _2MB_) {
        pte = (page_table_entry_t *)(*last_allocated_paddr);

        *last_allocated_paddr += PAGE_TABLE_SIZE;

        memset (pte, 0, PAGE_TABLE_SIZE);

        pde[pde_idx] = (phys_addr_t)pte | (PAGE_PRESENT | PAGE_RW);
      }
    }
    if (page_size == _4KB_) {
      pte = (page_table_entry_t *)((phys_addr_t)pde[pde_idx] & ~0xFFF);
      pte[pte_idx] = (phys_addr_t)paddr | protection;
    } else if (page_size == _2MB_) {
      pde[pde_idx] = (phys_addr_t)paddr | protection;
    } else {  /* 1GB Pages */
      pdpe[pdpe_idx] = (phys_addr_t)paddr | protection;
    }

    page++;
    paddr += page_size;
    vaddr += page_size;
  }
}
/* =============================================== */
void
EPT_map_memory (phys_addr_t *pml4_paddr,
                virt_addr_t memory_region_start_vaddr,
                virt_addr_t memory_region_end_vaddr,
                phys_addr_t memory_region_start_paddr,
                phys_addr_t *last_allocated_paddr,
                uint32_t page_size,
                uint16_t protection,
                int  flags)
{
  phys_addr_t paddr;
  virt_addr_t vaddr;
  page_table_entry_t *pml4 = NULL;
  page_table_entry_t *pdpe = NULL;
  page_table_entry_t *pde  = NULL;
  page_table_entry_t *pte  = NULL;
  int pml4_idx;
  int pdpe_idx;
  int pde_idx;
  int pte_idx;
  int page;
  int last_pages;
  size_t pages;
  size_t memory_region_size;

  *last_allocated_paddr = ALIGN ((*last_allocated_paddr), PAGE_TABLE_SIZE); 

  if (memory_region_start_vaddr > memory_region_end_vaddr) {
    panic ("mep_memory: memory start region can not be greater than end address");
  }

  if (memory_region_start_vaddr == memory_region_end_vaddr) {
    panic ("Can't map a memory region of zero size!");
  }

  if (flags == MAP_NEW) {
    pml4 = (page_table_entry_t *)(*last_allocated_paddr);
    
    *pml4_paddr = (phys_addr_t)pml4;

    *last_allocated_paddr += PAGE_TABLE_SIZE;

    memset (pml4, 0, PAGE_TABLE_SIZE);
  } else if (flags == MAP_UPDATE) {
    pml4 = (page_table_entry_t *)*pml4_paddr;

    if ((pml4[0] & (PAGE_PRESENT | PAGE_RW)) == 0) {
      cprintk ("Warning:\nSecond Boot Stage: Invalid PML4 used to map VMM Pages!", 0x4);
    }
    if (pml4[0] == 0) {
      panic ("Second Boot Stage: Trying to update null page tables!");
    }
  } else {
    panic ("Second Boot Stage: Undefined memory map flag!");
  }

  if (protection != VMM_PAGE_UNCACHABLE && protection != VMM_PAGE_NORMAL) {
    panic ("Second Boot Stage: Undefined VMM page protection!");
  }

  switch (page_size) {
    case _4KB_:
      break;
    case _2MB_:
    case _1GB_:
      protection |= PAGE_PSE;
      break;
    default:
      panic ("Second Boot Stage: Bad page size!");
  }

  memory_region_size =  memory_region_end_vaddr - memory_region_start_vaddr;

  pages = memory_region_size / page_size + (memory_region_size % page_size ? 1 : 0);

  paddr = memory_region_start_paddr;
  /*
   * In the whole stage2, virtual addresses are equal to
   * physical addresses
   */
  vaddr = memory_region_start_vaddr;
  /* TODO:
   *  1. Replace all constants here with some meaningfull macros.
   */ 
  page = 0;
  last_pages = pages;
  while (page != last_pages) {
    /* TODO:
     *     A boundry check to make sure none of indices go beyond
     *     512.
     */
    pml4_idx = (vaddr >> 39) & 0x1FF;
    pdpe_idx = (vaddr >> 30) & 0x1FF;
    pde_idx  = (vaddr >> 21) & 0x1FF;
    pte_idx  = (vaddr >> 12) & 0x1FF;


    if (pml4[pml4_idx] == 0) {
      pdpe = (page_table_entry_t *)(*last_allocated_paddr);

      *last_allocated_paddr += PAGE_TABLE_SIZE;

      memset (pdpe, 0, PAGE_TABLE_SIZE);

      pml4[pml4_idx] = (phys_addr_t)pdpe | 0x7; /* READ *//*| (PAGE_PRESENT | PAGE_RW)*/;
    }

    pdpe = (page_table_entry_t *)((phys_addr_t)pml4[pml4_idx] & ~0xFFF);
    if (pdpe[pdpe_idx] == 0 && page_size < _1GB_) {
      pde = (page_table_entry_t *)(*last_allocated_paddr);

      *last_allocated_paddr += PAGE_TABLE_SIZE;

      memset (pde, 0, PAGE_TABLE_SIZE);

      pdpe[pdpe_idx] = (phys_addr_t)pde | (PAGE_PRESENT | PAGE_RW);
    }
    if (page_size < _1GB_) {
      pde = (page_table_entry_t *)((phys_addr_t)pdpe[pdpe_idx] & ~0xFFF);

      if (pde[pde_idx] == 0 && page_size < _2MB_) {
        pte = (page_table_entry_t *)(*last_allocated_paddr);

        *last_allocated_paddr += PAGE_TABLE_SIZE;

        memset (pte, 0, PAGE_TABLE_SIZE);

        pde[pde_idx] = (phys_addr_t)pte | (PAGE_PRESENT | PAGE_RW);
      }
    }
    if (page_size == _4KB_) {
      pte = (page_table_entry_t *)((phys_addr_t)pde[pde_idx] & ~0xFFF);
      pte[pte_idx] = (phys_addr_t)paddr | protection;
    } else if (page_size == _2MB_) {
      pde[pde_idx] = (phys_addr_t)paddr | protection;
    } else {  /* 1GB Pages */
      pdpe[pdpe_idx] = (phys_addr_t)paddr | 0x7 | (1 << 7);/*| protection*/;
    }

    page++;
    paddr += page_size;
    vaddr += page_size;
  }
}

/* ========================================== */
#define VIRT2PHYS(vaddr) (                                    \
    (phys_addr_t)(                                        \
      ((phys_addr_t)(curr_vmm_phys_addr))     \
      +                                      \
      ((vaddr) - ((virt_addr_t)0x40000000))   \
      )                                        \
    )
/* ========================================== */
static void
load_all_vmms (phys_addr_t *first_free_addr, phys_addr_t vmm_elf_addr)
{
  Elf64_Ehdr *elf_hdr;  /* Start address of executable */
  Elf64_Phdr *s;

  phys_addr_t curr_vmm_phys_addr;
  //virt_addr_t curr_vmm_virt_addr;
  //virt_addr_t curr_vmm_vstack;

  size_t vmm_size;     /* VMM's Code + data + rodata + bss + stack */
  int ph_num;          /* VMM's number of program headers */
  int i;
  cpuid_t curr_cpu;
  struct cpu_info *curr_cpu_info;

  /* TODO:
   *    Replace constant value 0x400000 with a meaningful MACRO
   * XXX:
   *    I'm going to use variable curr_vmm_phys_addr in the macro
   *    VIRT2PHYS(). So be careful in using/altering/removing this
   *    variable. This is a bad hack, but it makes our code a lot
   *    easier to read.
   *    Before Using VIRT2PHYS() macro, make sure
   *    curr_vmm_phys_addr is assigned with proper value. Otherwise
   *    everything will break.
   *    To minimize the threat, I'm going to define VIRT2PHYS Macro
   *    after assignment of curr_vmm_phys_addr. Don't move or reorder them!
   */
  curr_vmm_phys_addr = ALIGN (*first_free_addr, _2MB_);
  for (curr_cpu = 0; curr_cpu < get_ncpus(); curr_cpu++) {
    //cprintk ("firest_free_addr = %x\n", 0x2, first_free_addr);
    curr_cpu_info = get_cpu_info (curr_cpu);
    curr_cpu_info->vmm_start_paddr = curr_vmm_phys_addr;
    cprintk ("curr vmm %x\n", 0xF, curr_vmm_phys_addr);
    elf_hdr = (Elf64_Ehdr*)vmm_elf_addr;  /* Start address of executable */
    s = (Elf64_Phdr*)((uint8_t*)vmm_elf_addr + elf_hdr->e_phoff);
    ph_num = elf_hdr->e_phnum;    /* Number of program headers in ELF executable */
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
    if (ph_num != 4) {
      cprintk ("ELF executable contains %d sections!\n", 0x4, ph_num);
      panic ("Unexpected VMM Header!\n");
    }
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
    //memset ((void*)VIRT2PHYS (s->p_vaddr), 0, s->p_memsz);
    /*
     * Here we want to reserve some space for the stack. Stack should be
     * 16 byte aligned!
     */
    curr_cpu_info->vmm_vstack = ALIGN ((virt_addr_t)s->p_vaddr + s->p_memsz, 16) + 0x1000;

    vmm_size = curr_cpu_info->vmm_vstack - curr_cpu_info->vmm_start_vaddr;

    curr_cpu_info->vmm_end_vaddr = curr_cpu_info->vmm_start_vaddr + vmm_size;

    if (vmm_size > VMM_MAX_SIZE) {
      panic ("Second Boot Stage: vmm is too large!");
    }

    curr_cpu_info->vmm_end_paddr = curr_cpu_info->vmm_start_paddr + vmm_size;

  //  cprintk ("curr_phys_addr = %x vmm_stack = %x vmm_end_addr %x\n", 0xE, curr_vmm_phys_addr, curr_cpu_info->vmm_vstack, curr_cpu_info->vmm_end_vaddr);
    /*
       cpus[i].vmm_start_paddr = ALIGN (cpus[i - 1].vmm_end_paddr, _2MB_);
       memset ((void *)cpus[0].vmm_start_paddr, 0, vmm_size*200);
       cprintk ("Done!\n", 0xA);
       halt ();*/
    curr_vmm_phys_addr += VMM_MAX_SIZE;
  } /* For */
  
  *first_free_addr = curr_vmm_phys_addr;

  for (curr_cpu = 0; curr_cpu < get_ncpus (); curr_cpu++) {
    curr_cpu_info = get_cpu_info (curr_cpu);

    map_memory (&curr_cpu_info->vmm_page_tables,
                 0, *first_free_addr,
                 0, 
                 first_free_addr,
                 _4KB_,
                 VMM_PAGE_NORMAL, MAP_NEW);

    map_memory (&curr_cpu_info->vmm_page_tables,
                 curr_cpu_info->vmm_start_vaddr, curr_cpu_info->vmm_end_vaddr,
                 curr_cpu_info->vmm_start_paddr, 
                 first_free_addr,
                 _4KB_,
                 VMM_PAGE_NORMAL, MAP_UPDATE);

    EPT_map_memory (&curr_cpu_info->vmm_ept_tables,
                 0, (phys_addr_t)0x40000000,
                 0, 
                 first_free_addr,
                 _1GB_,
                 VMM_PAGE_NORMAL, MAP_NEW);

  }
#undef VIRT2PHYS
}
/* ========================================== */
#define VIRT2PHYS(vaddr) (                                    \
    (phys_addr_t)(                                        \
      ((phys_addr_t)(curr_vm_phys_addr))     \
      +                                      \
      ((vaddr) - ((virt_addr_t)0x40000000))   \
      )                                        \
    )
/* ========================================== */
static void
load_all_vms (phys_addr_t *first_free_addr, phys_addr_t *vms_array)
{
  Elf64_Ehdr *elf_hdr;  /* Start address of executable */
  Elf64_Phdr *s;

  phys_addr_t curr_vm_phys_addr;
  //virt_addr_t curr_vmm_virt_addr;
  //virt_addr_t curr_vmm_vstack;

  size_t vm_size;     /* VMM's Code + data + rodata + bss + stack */
  int ph_num;          /* VMM's number of program headers */
  int i;
  cpuid_t curr_cpu;
  struct cpu_info *curr_cpu_info;

  /* TODO:
   *    Replace constant value 0x400000 with a meaningful MACRO
   * XXX:
   *    I'm going to use variable curr_vmm_phys_addr in the macro
   *    VIRT2PHYS(). So be careful in using/altering/removing this
   *    variable. This is a bad hack, but it makes our code a lot
   *    easier to read.
   *    Before Using VIRT2PHYS() macro, make sure
   *    curr_vmm_phys_addr is assigned with proper value. Otherwise
   *    everything will break.
   *    To minimize the threat, I'm going to define VIRT2PHYS Macro
   *    after assignment of curr_vmm_phys_addr. Don't move or reorder them!
   */
  curr_vm_phys_addr = ALIGN (*first_free_addr, _2MB_);
  cprintk ("curr_vm_phys_addr = %x\n", 0xA, curr_vm_phys_addr);
  /*
   * XXX ATTENTION:
   * XXX: This loop is starting from 1 (ONNNNNNNNNNNNNNNNNNNNNNNNNNE)
   * XXX: This loop is ends 3 (THREEEEEEEEEEEEEEEEEEEEEEEEE)
   */
  for (curr_cpu = 1; curr_cpu < 3; curr_cpu++) {
    //cprintk ("firest_free_addr = %x\n", 0x2, first_free_addr);
    curr_cpu_info = get_cpu_info (curr_cpu);
    curr_cpu_info->vm_start_paddr = curr_vm_phys_addr;
  cprintk ("vm_start_paddr = %x\n", 0xA, curr_cpu_info->vm_start_paddr);
    cprintk ("curr vm %x\n", 0xF, curr_vm_phys_addr);
    elf_hdr = (Elf64_Ehdr*)vms_array[curr_cpu - 1];  /* Start address of executable */
    s = (Elf64_Phdr*)((uint8_t*)vms_array[curr_cpu - 1] + elf_hdr->e_phoff);
    ph_num = elf_hdr->e_phnum;    /* Number of program headers in ELF executable */
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
    if (ph_num != 4) {
      cprintk ("ELF executable contains %d sections!\n", 0x4, ph_num);
      panic ("Unexpected VM Header!\n");
    }
    curr_cpu_info->vm_start_vaddr = (virt_addr_t)s->p_vaddr;
  cprintk ("vm_start_vaddr = %x\n", 0xA, curr_cpu_info->vm_start_vaddr);
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

      section_src_paddr = vms_array[curr_cpu - 1] + s->p_offset;  /* Address of section in executable */
      section_dst_paddr = VIRT2PHYS (s->p_vaddr);      /* Address of section when loaded in ram */
      section_size = (size_t)s->p_memsz;               /* Section size */

      if (section_size > 0) {
        memcpy ((void*)section_dst_paddr, (void*)section_src_paddr, section_size);
      }
      s++;    /* Go to next section */
    }
    /* Last section is bss. We zero out this section */
    //memset ((void*)VIRT2PHYS (s->p_vaddr), 0, s->p_memsz);
    /*
     * Here we want to reserve some space for the stack. Stack should be
     * 16 byte aligned!
     */
    curr_cpu_info->vm_vstack = ALIGN ((virt_addr_t)s->p_vaddr + s->p_memsz, 16) + 0x1000;

    curr_cpu_info->vm_vmxon_ptr = get_vmxon_ptr (curr_cpu_info->cpuid);
    curr_cpu_info->vm_vmcs_ptr  = get_vmcs_ptr  (curr_cpu_info->cpuid);

    vm_size = curr_cpu_info->vm_vstack - curr_cpu_info->vm_start_vaddr;

    curr_cpu_info->vm_end_vaddr = curr_cpu_info->vm_start_vaddr + vm_size;

    if (vm_size > VM_MAX_SIZE) {
      panic ("Second Boot Stage: vm is too large!");
    }

    curr_cpu_info->vm_end_paddr = curr_cpu_info->vm_start_paddr + vm_size;

  //  cprintk ("curr_phys_addr = %x vmm_stack = %x vmm_end_addr %x\n", 0xE, curr_vmm_phys_addr, curr_cpu_info->vmm_vstack, curr_cpu_info->vmm_end_vaddr);
    /*
       cpus[i].vmm_start_paddr = ALIGN (cpus[i - 1].vmm_end_paddr, _2MB_);
       memset ((void *)cpus[0].vmm_start_paddr, 0, vmm_size*200);
       cprintk ("Done!\n", 0xA);
       halt ();*/
    curr_vm_phys_addr += VM_MAX_SIZE;
  } /* For */
  /* XXX: ATTENTION
   *
   * This loop starts from 1 (ONNNNNNNNNNNNNNNNNNNNNNNNNNNNNNE) to 2 TWWWWWWWWWWWWWWWWWWWWWWWO
   */
  *first_free_addr = curr_vm_phys_addr;
  for (curr_cpu = 1; curr_cpu < 3; curr_cpu++) {
    curr_cpu_info = get_cpu_info (curr_cpu);

    map_memory (&curr_cpu_info->vm_page_tables,
        0, *first_free_addr,
        0, 
        first_free_addr,
        _4KB_,
        VM_PAGE_NORMAL, MAP_NEW);

    map_memory (&curr_cpu_info->vm_page_tables,
        curr_cpu_info->vm_start_vaddr, curr_cpu_info->vm_end_vaddr,
        curr_cpu_info->vm_start_paddr, 
        first_free_addr,
        _4KB_,
        VM_PAGE_NORMAL, MAP_UPDATE);
  }
 
#undef VIRT2PHYS
}

void 
boot_stage2_main (struct boot_stage2_args *boot_args)
{
  phys_addr_t first_free_addr;
  phys_addr_t stage2_page_tables;
  phys_addr_t vmm_elf_addr;
  phys_addr_t vms_array[] = { boot_args->pm_elf_addr, boot_args->fs_elf_addr};

  first_free_addr    = boot_args->boot_stage2_end_addr;
  stage2_page_tables = boot_args->boot_stage2_page_tables;
  vmm_elf_addr       = boot_args->vmm_elf_addr;

  con_init ();

  cprintk ("stage2_end_addr = %x\n", 0x6, first_free_addr);
  /*
   * XXX:
   *    map_iomem will change the end address of stage2 program.
   *    Therefore, stage2_end_addr is sent by reference.
   *    This is important because later in load_all_vmm, we need
   *    to know the correct end address of stage2 program.
   */
  cprintk ("stage2_end_addr = %x\n", 0x6, first_free_addr);
  map_memory (&stage2_page_tables, 0xFEC00000, 0x100000000, 0xFEC00000, &first_free_addr, _2MB_, VMM_PAGE_UNCACHABLE, MAP_UPDATE);
  //map_iomem (stage2_pml4, &stage2_end_addr);
  cprintk ("stage2_end_addr = %x\n", 0x6, first_free_addr);

  vmx_data_structure_pool = (phys_addr_t)(ALIGN (first_free_addr, 0x1000) + (get_ncpus () * 0x1000 * 2));
  first_free_addr = vmx_data_structure_pool;

  mp_init ();

  interrupt_init ();

  pic_init ();
  ioapic_init ();
  lapic_init();
  kbd_init ();
  load_all_vmms (&first_free_addr, vmm_elf_addr);
  load_all_vms (&first_free_addr, vms_array);
#if 0
  int i;
  for (i = 0; i < get_ncpus(); i++) {
    cprintk ("--stack %x ", 0xE, cpus[i].vmm_vstack);
    cprintk ("--vmm_start %x\n", 0xE, cpus[i].vmm_start_vaddr);
  }
#endif
  mp_bootothers ();
  __asm__ __volatile__ ("sti\n");

  for (;;);
}

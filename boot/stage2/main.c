#include <cdef.h>
#include <dev/pic.h>
#include <types.h>
#include <kernel_args.h>
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
#define ALIGN(addr, bound) (((addr)+((bound)-1))&(~((bound)-1)))
/* ========================================== */
#define _2MB_ 0x200000
#define _4MB_ (2 * _2MB_)
#define _6MB_ (3 * _2MB_)
/* ========================================== */
void
map_memory (phys_addr_t pml4_paddr,
            phys_addr_t memory_region_start_paddr,
            phys_addr_t memory_region_end_paddr,
            phys_addr_t *last_allocated_paddr)
{
  phys_addr_t paddr;
  virt_addr_t vaddr;
  page_table_entry_t *pml4;
  page_table_entry_t *pdpe;
  page_table_entry_t *pde;
  int pml4_idx;
  int pdpe_idx;
  int pde_idx;
  int page;
  int last_pages;
  size_t pages;
  size_t memory_region_size;


  *last_allocated_paddr = ALIGN ((*last_allocated_paddr), PAGE_TABLE_SIZE); 

  if (memory_region_start_paddr > memory_region_end_paddr) {
    panic ("mep_memory: memory start region can not be greater than end address");
  }

  if (memory_region_start_paddr == memory_region_end_paddr)
    return;

  memory_region_size =  memory_region_end_paddr - memory_region_start_paddr;

  pages = memory_region_size / KNL_PAGE_SIZE + (memory_region_size % KNL_PAGE_SIZE ? 1 : 0);

  pml4 = (page_table_entry_t *)pml4_paddr;

  paddr = memory_region_start_paddr;
  /*
   * In the whole kernel, virtual addresses are equal to
   * physical addresses
   */
  vaddr = paddr;
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

    pdpe = (page_table_entry_t *)((phys_addr_t)pml4[pml4_idx] & ~0xFFF);
    pde = (page_table_entry_t *)((phys_addr_t)pdpe[pdpe_idx] & ~0xFFF);

    if (pml4[pml4_idx] == 0) {
      panic ("Trying to map addresses into empty page tables!");
    }
    if (pdpe[pdpe_idx] == 0) {
      pde = (page_table_entry_t *)(*last_allocated_paddr);

      *last_allocated_paddr += PAGE_TABLE_SIZE;

      memset (pde, 0, PAGE_TABLE_SIZE);

      pdpe[pdpe_idx] = (phys_addr_t)pde  | (PAGE_PRESENT | PAGE_RW);
    }
    pde[pde_idx] = paddr | (PAGE_PRESENT | PAGE_RW | PAGE_PSE | PAGE_PCD); /* Page is not cachable */
    page++;
    paddr += KNL_PAGE_SIZE;
    vaddr += KNL_PAGE_SIZE;
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
load_all_vmms (phys_addr_t first_free_addr, phys_addr_t vmm_elf_addr)
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
  curr_vmm_phys_addr = ALIGN (first_free_addr, _2MB_);
  for (curr_cpu = 0; curr_cpu < get_ncpus(); curr_cpu++) {
    //cprintk ("firest_free_addr = %x\n", 0x2, first_free_addr);
    curr_cpu_info = get_cpu_info (curr_cpu);
    curr_cpu_info->vmm_start_paddr = curr_vmm_phys_addr;
    elf_hdr = (Elf64_Ehdr*)vmm_elf_addr;  /* Start address of executable */
    s = (Elf64_Phdr*)((uint8_t*)vmm_elf_addr + elf_hdr->e_phoff);
    ph_num = elf_hdr->e_phnum;    /* Number of program headers in ELF executable */
    /*
     * Kernel ELF header contains 4 sections. (look at kernel/link64.ld).
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

    if (vmm_size > _2MB_) {
      panic ("vmm is bigger than expected!");
    }

    curr_cpu_info->vmm_end_paddr = curr_cpu_info->vmm_start_paddr + vmm_size;

    cprintk ("stack %x ", 0xE, curr_cpu_info->vmm_vstack);
    cprintk ("vmm_start %x\n", 0xE, curr_cpu_info->vmm_start_paddr);
    /*
       cpus[i].vmm_start_paddr = ALIGN (cpus[i - 1].vmm_end_paddr, _2MB_);
       memset ((void *)cpus[0].vmm_start_paddr, 0, vmm_size*200);
       cprintk ("Done!\n", 0xA);
       halt ();*/
    curr_vmm_phys_addr += _2MB_;
  } /* For */
  for (i = 0; i < get_ncpus (); i++) {
    //cprintk ("cpus[%d].vmm_start_Addr = %x\n", 0x3, i, cpus[i].vmm_start_paddr);
  }
#if 0
  /*
   * Create page tables
   */
  page_table_entry_t *pml4;
  page_table_entry_t *pdpe;
  page_table_entry_t *pde1;
  page_table_entry_t *pde2;
  int pml4_idx;
  int pdpe_idx;
  int pde_idx;
  curr_vmm_vstack = 0; /*XXX*/
  curr_vmm_virt_addr = 0; /* XXX */
  pml4 = (page_table_entry_t *)ALIGN (VIRT2PHYS (curr_vmm_vstack), PAGE_TABLE_SIZE);
  pdpe = (page_table_entry_t *)((phys_addr_t)pml4 + PAGE_TABLE_SIZE);
  /* pde1 is used to map VMM's virtual address */
  pde1  = (page_table_entry_t *)((phys_addr_t)pdpe + PAGE_TABLE_SIZE);
  /* pde2 is used to map first 4MB of memory for VMM */
  pde2  = (page_table_entry_t *)((phys_addr_t)pde1 + PAGE_TABLE_SIZE);
  //cprintk ("vmm start = %x\nvmm_end = %x\npml4 = %x\npdpe = %x\npde = %x\n", 0x5, curr_vmm_phys_addr, *vmm_end_paddr, pml4, pdpe, pde1);
  //cprintk("Stack virtual address = %x\n", 0x4, curr_vmm_vstack);
  pml4_idx = (curr_vmm_virt_addr >> PML4_SHIFT) & 0x1FF;  /* idx in pml4 */
  pdpe_idx = (curr_vmm_virt_addr >> PDPE_SHIFT) & 0x1FF;  /* idx in pdpe */
  pde_idx  = (curr_vmm_virt_addr >> PDE_SHIFT) & 0x1FF;  /* pde idx in pde1 */
  //cprintk ("pml4_idx = %x\npdpe_idx = %x\npde_idx = %x\n", 0x5, pml4_idx, pdpe_idx, pde_idx);

  pml4[pml4_idx] = ((phys_addr_t)pdpe) | PAGE_PRESENT;
  pdpe[pdpe_idx] = ((phys_addr_t)pde1) | PAGE_PRESENT;
  pde1[pde_idx]   = ((phys_addr_t)curr_vmm_phys_addr) | (PAGE_PRESENT | PAGE_RW | PAGE_PSE);
  /* Map first 4MB of memory for all VMMs */
  pdpe[0] = ((phys_addr_t)pde2) | PAGE_PRESENT;
  pde2[0] = 0 | (PAGE_PRESENT | PAGE_RW | PAGE_PSE);
  pde2[1] = 0x200000 | (PAGE_PRESENT | PAGE_RW | PAGE_PSE);
  //cprintk ("pml4 -> %x\n pdpe -> %x pde %x\n", 0x6, pml4[pml4_idx], pdpe[pdpe_idx], pde1[pde_idx]);

  //cprintk ("size of vmm image = %x\n", 0xA, ((uint64_t)pde2 + 0x1000) - VIRT2PHYS (0x40000000));

  cpus[0].vmm_vstack = curr_vmm_vstack;
  cpus[0].vmm_page_tables   = (phys_addr_t)pml4;
  cpus[0].vmm_start_paddr = 0x400000;

  /*
   * Here we have some VERY beautiful lines of code. So become prepared :)
   * So far in this function, we parsed the ELF header, and put its each section
   * in the right memory address, and created the stack for the first VMM and also
   * the page tables.
   *
   * Now at this point, to avoid duplication, instead of parsing ELF header again, we
   * are going to copy the same loaded VMM "ncpus" times, in adjacent memory locations.
   * 
   * Of course here, we need to adapt all the page tables for each core.
   */
  if (vmm_size >= 0x200000) {
    panic ("Too big VMM");
  }
  // phys_addr_t cpu_addr[] = {0x400000, 0x600000, 0x800000, 0xA00000, 0xC00000, 0xE00000, 0x1100000, 0x1300000};
  uint64_t j;
  for (j = 0; j < 15; j++) {
    cpus[i].vmm_start_paddr = 0x600000 + 0x200000 * j;
    cprintk ("i = %d cpus = %x, cpus[%d].vmm_start_paddr = %x\n", 0xA, j, cpus, j, &cpus[j].vmm_start_paddr);
  }
#endif
#undef VIRT2PHYS
}
#if 0
  static void
print_logo(void)
{
  cprintk("Welcome to stage2\n", 0xB);
  cprintk("       v       v  u    u         oo      sss\n", 0xB);
  cprintk("        v     v   u    u        o  o    s\n", 0xB);
  cprintk("         v   v    u    u  ===   o  o     s\n", 0xB);
  cprintk("          v v     u    u        o  o      s\n", 0xB);
  cprintk("           v       uuuu          oo     sss\n", 0xB);
  cprintk("\n", 0xB);
}
#endif
  void 
kmain (struct kernel_args *stage1_info)
{
  phys_addr_t stage2_end_addr;
  phys_addr_t stage2_pml4;
  phys_addr_t vmm_elf_addr;

  stage2_end_addr = stage1_info->ka_kernel_end_addr;
  stage2_pml4     = stage1_info->ka_kernel_cr3;
  vmm_elf_addr    = stage1_info->ka_init_addr;

  con_init ();

  cprintk ("stage2_end_addr = %x\n", 0x6, stage2_end_addr);
  /*
   * XXX:
   *    map_iomem will change the end address of stage2 program.
   *    Therefore, stage2_end_addr is sent by reference.
   *    This is important because later in load_all_vmm, we need
   *    to know the correct end address of stage2 program.
   */
  map_memory (stage2_pml4, 0xFEC00000, 0x100000000, &stage2_end_addr);
  //map_iomem (stage2_pml4, &stage2_end_addr);
  cprintk ("stage2_end_addr = %x\n", 0x6, stage2_end_addr);

  mp_init ();

  interrupt_init ();

  pic_init ();
  ioapic_init ();
  lapic_init();
  kbd_init ();

  load_all_vmms (stage2_end_addr, vmm_elf_addr);
#if 0
  int i;
  for (i = 0; i < get_ncpus(); i++) {
    cprintk ("--stack %x ", 0xE, cpus[i].vmm_vstack);
    cprintk ("--vmm_start %x\n", 0xE, cpus[i].vmm_start_vaddr);
  }
#endif
  halt ();
  mp_bootothers ();
  __asm__ __volatile__ ("sti\n");

  for (;;);
}

#include <cdef.h>
#include <dev/pic.h>
#include <types.h>
#include <kernel_args.h>
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
#include <elf.h>
#include <string.h>
#include <panic.h>
#include <page.h>
/* ========================================== */
#define ALIGN(addr, bound) (((addr)+((bound)-1))&(~((bound)-1)))
/* ========================================== */
extern struct cpu cpus[MAX_CPUS];
/* ========================================== */
static void
load_vmm (phys_addr_t vmm_elf_addr, 
          phys_addr_t *vmm_end_paddr)
{
	Elf64_Ehdr *elf_hdr;  /* Start address of executable */
	Elf64_Phdr *s;
  phys_addr_t curr_vmm_phys_addr;
  virt_addr_t curr_vmm_virt_addr;
  virt_addr_t curr_vmm_vstack;
  int ph_num;
  int i;

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
  curr_vmm_phys_addr = 0x400000;
/* ========================================== */
#define VIRT2PHYS(vaddr) (                                    \
        (phys_addr_t)(                                        \
                      ((phys_addr_t)(curr_vmm_phys_addr))     \
                       +                                      \
                      ((vaddr) - ((virt_addr_t)0x40000000))   \
                     )                                        \
)
/* ========================================== */

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
    panic ("Unexpected VMM Header!\n");
  }
  curr_vmm_virt_addr = (virt_addr_t)s->p_vaddr;
  cprintk ("code vaddr = %x\n", 0x3, curr_vmm_virt_addr);
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
    /* If section contains bytes, we copy them into the proper memory address */
    cprintk ("%dth section_size = %d\tdst addr%x\n", 0x5, i, section_size, section_dst_paddr);
    cprintk ("%dth vaddr = %x\n", 0x3, i, s->p_vaddr);
    if (section_size > 0) {
      memcpy ((void*)section_dst_paddr, (void*)section_src_paddr, section_size);
    }
    s++;    /* Go to next section */
  }
  /* Last section is bss. We zero out this section */
  memset ((void*)VIRT2PHYS (s->p_vaddr), 0, s->p_memsz);
  cprintk ("bss size = %d\n", 0x5, s->p_memsz);
  cprintk ("bss vaddr = %x\n", 0x3,  s->p_vaddr);
  /*
   * Here we want to reserve some space for the stack. Stack should be
   * 16 byte aligned!
   */
  curr_vmm_vstack = ALIGN ((virt_addr_t)s->p_vaddr + s->p_memsz, 16) + 0x1000;
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

  pml4 = (page_table_entry_t *)ALIGN (VIRT2PHYS (curr_vmm_vstack), PAGE_TABLE_SIZE);
  pdpe = (page_table_entry_t *)((phys_addr_t)pml4 + PAGE_TABLE_SIZE);
  /* pde1 is used to map VMM's virtual address */
  pde1  = (page_table_entry_t *)((phys_addr_t)pdpe + PAGE_TABLE_SIZE);
  /* pde2 is used to map first 4MB of memory for VMM */
  pde2  = (page_table_entry_t *)((phys_addr_t)pde1 + PAGE_TABLE_SIZE);
  //cprintk ("vmm start = %x\nvmm_end = %x\npml4 = %x\npdpe = %x\npde = %x\n", 0x5, curr_vmm_phys_addr, *vmm_end_paddr, pml4, pdpe, pde1);
  cprintk("Stack virtual address = %x\n", 0x4, curr_vmm_vstack);
  pml4_idx = (curr_vmm_virt_addr >> PML4_SHIFT) & 0x1FF;  /* idx in pml4 */
  pdpe_idx = (curr_vmm_virt_addr >> PDPE_SHIFT) & 0x1FF;  /* idx in pdpe */
  pde_idx  = (curr_vmm_virt_addr >> PDE_SHIFT) & 0x1FF;  /* pde idx in pde1 */
  cprintk ("pml4_idx = %x\npdpe_idx = %x\npde_idx = %x\n", 0x5, pml4_idx, pdpe_idx, pde_idx);

  pml4[pml4_idx] = ((phys_addr_t)pdpe) | PAGE_PRESENT;
  pdpe[pdpe_idx] = ((phys_addr_t)pde1) | PAGE_PRESENT;
  pde1[pde_idx]   = ((phys_addr_t)curr_vmm_phys_addr) | (PAGE_PRESENT | PAGE_RW | PAGE_PSE);
  /* Map first 4MB of memory for all VMMs */
  pdpe[0] = ((phys_addr_t)pde2) | PAGE_PRESENT;
  pde2[0] = 0 | (PAGE_PRESENT | PAGE_RW | PAGE_PSE);
  pde2[1] = 0x200000 | (PAGE_PRESENT | PAGE_RW | PAGE_PSE);
  cprintk ("pml4 -> %x\n pdpe -> %x pde %x\n", 0x6, pml4[pml4_idx], pdpe[pdpe_idx], pde1[pde_idx]);
  

  cpus[1].stack = curr_vmm_vstack;
  cpus[1].page_tables   = (phys_addr_t)pml4;
  /* Shows where this VMM ends! */
  /* XXX:
   * If you add anything after pde2 in VMM's state in memory,
   * make sure to update this line of code. 
   * Currently, pde2 is the last part of memory of a loaded VMM.
   */
  *vmm_end_paddr = (phys_addr_t)pde2;    
/* We don't need this "dangerous macro any more */
#undef VIRT2PHYS
}
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
void 
kmain (struct kernel_args *kargs)
{
  con_init ();
  mm_init (kargs->ka_kernel_end_addr, kargs->ka_kernel_cr3, kargs->ka_memsz);
#if 0
  uint32_t eax, ebx, ecx, edx;
  cpuid (1, &eax, NULL, &ecx, NULL);
  if (ecx & (1 << 5)) {
    cprintk ("VMX is supported!\n", 0xA);
  } else {
    cprintk ("VMX is NOT supported!\n", 0x4);
  }
  cpuid (1, &eax, NULL, &ecx, NULL);
  if (ecx & (1 << 3)) {
    cprintk ("Monitor/Mwait is supported!\n", 0xA);
  } else {
    cprintk ("Monitor/Mwait is NOT supported!\n", 0x4);
  }
	cpuid(0x80000008, &eax, &ebx, &ecx, &edx);
	cprintk ("Physical Address Bits: %d\n", 0xE, eax & 0x000000FF);
	cprintk ("Cores per Die: %d\n", 0xE, (ecx & 0x000000FF) + 1);
#endif
  mp_init ();

  interrupt_init ();

  pic_init ();
  ioapic_init ();
  lapic_init();
  kbd_init ();

  print_logo();
  phys_addr_t vmm_end_addr;
  load_vmm (kargs->ka_init_addr, &vmm_end_addr);
  //vmm (1);
  //cprintk ("kinit address %x\n", 0xA, kargs->ka_init_addr);
  //__asm__ __volatile__ ("jmp .\n\t");
  mp_bootothers ();
  halt ();
  __asm__ __volatile__ ("sti\n");

  for (;;);
}

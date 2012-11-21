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
#define ALIGN(addr, bound) (((addr)+((bound)-1))&(~((bound)-1)))
/* ========================================== */
static void
load_vmm (phys_addr_t vmm_elf_addr, 
             void (**vmm)(int cpuid),
             phys_addr_t *vmm_end_paddr)
{
	Elf64_Ehdr *elf_hdr = (Elf64_Ehdr*)vmm_elf_addr;  /* Start address of executable */
	Elf64_Phdr *s = (Elf64_Phdr*)((uint8_t*)vmm_elf_addr + elf_hdr->e_phoff);
  int ph_num = elf_hdr->e_phnum;    /* Number of program headers in ELF executable */
  int i;
  /*
   * Kernel ELF header contains 4 sections. (look at kernel/link64.ld).
   * These sections are respectively
   *   1. text
   *   2. data
   *   3. rodata
   *   4. bss
   * If we first check to make sure we have exactly four sections in the
   * ELF file loaded by grub.
   * Its also best if we check to make sure that each of these sections
   * contains exactly the content we expect. This is possible by checking
   * the type field of each section. But for simplicity I assume that their
   * contents are OK at the moment.
   */
  if (ph_num != 4) {
    panic ("Unexpected Kernel Header!\n");
  }
  /* After loading all sections of kernel in the right address,
   * boot loader will transfer the control to kernel, by calling
   * the "kernel" function pointer which is assigned here. */
  phys_addr_t vmm_phys_addr = (phys_addr_t)s->p_vaddr + 0x200000;
  virt_addr_t vmm_virt_addr = (virt_addr_t)s->p_vaddr;
  *vmm = (void (*)(int cpuid))(vmm_phys_addr);
  cprintk ("code vaddr = %x\n", 0x3, s->p_vaddr);
  /* Why Upper bound is ph_num - 1?
   * Because in kernel ELF executable, last section is .bss which does not have any byes.
   * So we don't need to copy any bytes from this section into memory. But we do need to allocate
   * enough memory for this section when we load the executable, and we
   * also need to zero out allocated memory. That's why we put .bss section at the
   * end of executable.
   */
  for (i = 0; i < ph_num - 1; i++) {
    phys_addr_t section_src_paddr = vmm_elf_addr + s->p_offset;      /* Address of section in executable */
    phys_addr_t section_dst_paddr = (phys_addr_t)s->p_vaddr + 0x200000;  /* Address of section when loaded in ram */
    size_t section_size = (size_t)s->p_memsz;                /* Section size */
    /* If section contains bytes, we copy them into the proper memory address */
    cprintk ("%dth section_size = %d\tdst addr%x\n", 0x5, i, section_size, section_dst_paddr);
    cprintk ("%dth vaddr = %x\n", 0x3, i, s->p_vaddr);
    if (section_size > 0) {
      memcpy ((void*)section_dst_paddr, (void*)section_src_paddr, section_size);
    }
    s++;    /* Go to next section */
  }
  /* Last section is bss. We zero out this section */
  memset ((void*)((phys_addr_t)s->p_vaddr) + 0x200000, 0, s->p_memsz);
  cprintk ("bss size = %d\n", 0x5, s->p_memsz);
  cprintk ("bss vaddr = %x\n", 0x3,  s->p_vaddr);
  *vmm_end_paddr = s->p_vaddr + s->p_memsz + 0x200000;    /* Shows where kernel ends! */
  /*
   * Here we want to reserve some space for the stack. Stack should be
   * 16 byte aligned!
   */
  phys_addr_t stack_base_paddr = ALIGN ((phys_addr_t)*vmm_end_paddr, 16);
  *vmm_end_paddr = stack_base_paddr + 0x1000;
  /*
   * Create page tables
   */
  page_table_entry_t *pml4 = (page_table_entry_t *)ALIGN ((phys_addr_t)*vmm_end_paddr, 0x1000);
  page_table_entry_t *pdpe = (page_table_entry_t *)((phys_addr_t)pml4 + 0x1000);
  page_table_entry_t *pde  = (page_table_entry_t *)((phys_addr_t)pdpe + 0x1000);
  int pml4_idx;
  int pdpe_idx;
  int pde_idx;
  cprintk ("vmm start = %x\nvmm_end = %x\npml4 = %x\npdpe = %x\npde = %x\n", 0x5, vmm_phys_addr, *vmm_end_paddr, pml4, pdpe, pde);
  cprintk("Stack virtual address = %x\n", 0x4, (phys_addr_t)pml4 - 0x200000);

  pml4_idx = (vmm_virt_addr >> 39) & 0x1FF;
  pdpe_idx = (vmm_virt_addr >> 30) & 0x1FF;
  pde_idx  = (vmm_virt_addr >> 21) & 0x1FF;
  cprintk ("pml4_idx = %x\npdpe_idx = %x\npde_idx = %x\n", 0x5, pml4_idx, pdpe_idx, pde_idx);

  pml4[pml4_idx] = ((phys_addr_t)pml4 + 0x1000) | PAGE_PRESENT;
  pdpe[pdpe_idx] = ((phys_addr_t)pdpe + 0x1000) | PAGE_PRESENT;
  pde[pde_idx]   = ((phys_addr_t)vmm_phys_addr) | (PAGE_PRESENT | PAGE_RW | PAGE_PSE);
  pde[0] = 0 | (PAGE_PRESENT | PAGE_RW | PAGE_PSE);
  cprintk ("pml4 -> %x\n pdpe -> %x pde %x\n", 0x6, pml4[pml4_idx], pdpe[pdpe_idx], pde[pde_idx]);
  cprintk ("Stack address = %x\n", 0x9, stack_base_paddr);
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
  void (*vmm)(int cpuid);
  phys_addr_t vmm_end_addr;
  load_vmm (kargs->ka_init_addr, &vmm, &vmm_end_addr);
  //vmm (1);
  //cprintk ("kinit address %x\n", 0xA, kargs->ka_init_addr);
  //__asm__ __volatile__ ("jmp .\n\t");
  mp_bootothers ();
  halt ();
  __asm__ __volatile__ ("sti\n");

  for (;;);
}

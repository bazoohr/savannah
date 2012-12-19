/* ==================================== *
 * loader.c                             *
 *                                      *
 * Hamid R. Bazoobandi                  *
 * 14 July 2012 Amsterdam               *
 * ==================================== */
#include <types.h>
#include <elf.h>
#include <string.h>
#include <kernel_args.h>
#include "screen.h"
#include "bootio.h"
#include "multiboot.h"
#include "asm.h"

#define ALIGN(addr, bound) (((addr)+((bound)-1))&(~((bound)-1)))
#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))
void
cpuid(uint32_t info, uint32_t *eaxp, uint32_t *ebxp,
      uint32_t *ecxp, uint32_t *edxp)
{
	uint32_t eax, ebx, ecx, edx;
	__asm volatile("cpuid" 
		: "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
		: "a" (info));
	if (eaxp)
		*eaxp = eax;
	if (ebxp)
		*ebxp = ebx;
	if (ecxp)
		*ecxp = ecx;
	if (edxp)
		*edxp = edx;
}

static struct kernel_args kargs;
/*
 * Kernel ELF executable is loaded into memory by Grub. It is not
 * yet ready for execution. Boot loader should read the ELF header
 * and copy each section of executable into the right place of memory
 * before the Kernel can run.
 * This function is supposed to do that.
 */
static void
load_kernel (phys_addr_t kernel_elf_addr, 
             void (**kernel)(struct kernel_args *),
             phys_addr_t *kernel_end_addr)
{
	Elf64_Ehdr *elf_hdr = (Elf64_Ehdr*)kernel_elf_addr;  /* Start address of executable */
	Elf64_Phdr *s = (Elf64_Phdr*)((uint8_t*)kernel_elf_addr + elf_hdr->e_phoff);
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
    printf ("Unexpected Kernel Header!\n");
    halt ();
  }
  /* After loading all sections of kernel in the right address,
   * boot loader will transfer the control to kernel, by calling
   * the "kernel" function pointer which is assigned here. */
  *kernel = (void (*)(struct kernel_args *))((phys_addr_t)s->p_vaddr);
  /* Why Upper bound is ph_num - 1?
   * Because in kernel ELF executable, last section is .bss which does not have any byes.
   * So we don't need to copy any bytes from this section into memory. But we do need to allocate
   * enough memory for this section when we load the executable, and we
   * also need to zero out allocated memory. That's why we put .bss section at the
   * end of executable.
   */
  for (i = 0; i < ph_num - 1; i++) {
    phys_addr_t section_src_addr = kernel_elf_addr + s->p_offset;      /* Address of section in executable */
    phys_addr_t section_dst_addr = (phys_addr_t)s->p_vaddr;  /* Address of section when loaded in ram */
    size_t section_size = (size_t)s->p_memsz;                /* Section size */
    /* If section contains bytes, we copy them into the proper memory address */
    if (section_size > 0) {
      bcopy ((void*)section_src_addr, (void*)section_dst_addr, section_size);
    }
    s++;    /* Go to next section */
  }
  /* Last section is bss. We zero out this section */
  memset ((void*)((phys_addr_t)s->p_vaddr), 0, s->p_memsz);
  *kernel_end_addr = s->p_vaddr + s->p_memsz;    /* Shows where kernel ends! */
}
void
boot_loader (unsigned long magic, unsigned long addr)
{
  multiboot_info_t *mbi = (multiboot_info_t *)addr;
  phys_addr_t init_elf_addr          = 0;
  phys_addr_t kernel_elf_addr       = 0;
  phys_addr_t boot_aps_bin_addr     = 0;
  phys_addr_t kernel_end_addr       = 0;
  size_t memory_size                = 0;
  void (*kernel)(struct kernel_args *);

  clrscr ();  /* Clear the screen. */
#if 0
  uint32_t rax;
  uint32_t rbx;
  uint32_t rcx;
  uint32_t rdx;

  cpuid (1, &rax, &rbx, &rcx, &rdx);
  printf ("rax = %x\n", rax);
  printf ("rbx = %x\n", rbx);
  printf ("rcx = %x\n", rcx);
  printf ("rcx = %x\n", rdx);
  if (rcx & 0x1) {
    printf ("SSE, SSE1 supported!\n");
  } else {
    printf ("SSE, SSE1 NOT supported!\n");
  }
  if (rcx & (1<<3)) {
    printf ("MWAIT supported!\n");
  } else {
    printf ("MWAIT NOT supported!\n");
  }

  if (rcx & (1<<26)) {
    printf ("SSSE3 is supported!\n");
  } else {
    printf ("SSSE3 NOT supported!\n");
  }
  /* Am I booted by a Multiboot-compliant boot loader? */
  if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
    printf ("ERROR: Invalid magic number: 0x%x\n", (unsigned) magic);
    halt ();
  }
#endif
  /* Does CPU have longmode? */
  if (has_long_mode () == false) {
    printf ("ERROR: Your system is not supporting long mode!\n");
    halt ();
  }
  if (has_x87_and_media_support () == true) {
    enable_x87_and_media ();
  } else {
    printf ("ERROR: Your system does not support X87 and media instructions.\n");
    halt ();
  }
  if (has_1GBpage () == false) {
    /* Then use 1GB pages to map all the memory */
  }
  /* Are mem_* valid? */
  if (CHECK_FLAG (mbi->flags, 0)) {
    memory_size = mbi->mem_upper;    /* Size of memory in KB */
    memory_size++;
  } else {
    printf ("ERROR: Size of memory is not known!\n");
    halt ();
  }

  /* Kernel should be loaded as a multi-boot module */
  if (CHECK_FLAG (mbi->flags, 3)) {
    multiboot_module_t *mod;
    if (mbi->mods_count != 3) {
      printf ("ERROR: Grub failed to load all needed modules. %d modules loaded, but 3 were needed!\n", mbi->mods_count);
      halt ();
    }

    mod = (multiboot_module_t*)mbi->mods_addr;
/* Init is supposed to be VMM */
    init_elf_addr = (phys_addr_t)mod->mod_start;
    if (mod->mod_end > 0x200000) {
      printf ("ERROR: loaded module overlaps with VMM start address\n");
      halt ();
    }
    mod++;
    boot_aps_bin_addr = (phys_addr_t)mod->mod_start;
    if (mod->mod_end > 0x200000) {
      printf ("ERROR: loaded module overlaps with VMM start address\n");
      halt ();
    }

    /*
     * The important thing to consider here is that
     * the boot_aps program, must not be bigger than
     * 512 bytes.
     *
     * The address must be 4kb aligned!
     * TODO:
     *     Define 0x9F000 as a macro to some good place.
     */
    memcpy ((void*)0x9F000, (void*)boot_aps_bin_addr, 512);

    mod++;
    kernel_elf_addr = (phys_addr_t)mod->mod_start;
    if (mod->mod_end > 0x200000) {
      printf ("ERROR: loaded module overlaps with kernel start address\n");
      halt ();
    }

  } else {
    printf ("ERROR: Kernel is not loaded as multiboot!\n");
    halt ();
  }
  /* 
   * Now we put each section of kernel in the right address
   * so that we can run the kernel.
   */
  load_kernel (kernel_elf_addr, &kernel, &kernel_end_addr);
  /* we pass several arguments to kernel */
  kargs.ka_memsz            = memory_size;           /* First argument: size of memory */
  kargs.ka_kernel_end_addr  = kernel_end_addr;   /* Third argument: start address of page tables */
  kargs.ka_init_addr        = init_elf_addr;
  kernel (&kargs);    /* Call kernel */
}

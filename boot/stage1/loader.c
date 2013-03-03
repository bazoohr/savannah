/* ==================================== *
 * loader.c                             *
 *                                      *
 * Hamid R. Bazoobandi                  *
 * 14 July 2012 Amsterdam               *
 * ==================================== */
#include <types.h>
#include <elf.h>
#include <string.h>
#include <boot_stage2_args.h>
#include <cpu.h>
#include <vuos/config.h>
#include "screen.h"
#include "bootio.h"
#include "multiboot.h"
#include "asm.h"

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

static struct boot_stage2_args stage2_args;
/*
 * boot stage2 ELF executable is loaded into memory by Grub. It is not
 * yet ready for execution. Boot loader should read the ELF header
 * and copy each section of executable into the right place of memory
 * before the stage2 can run.
 * This function is supposed to do that.
 */
static void
load_stage2 (phys_addr_t stage2_elf_addr,
             void (**stage2)(struct boot_stage2_args *),
             phys_addr_t *stage2_end_addr)
{
	Elf64_Ehdr *elf_hdr = (Elf64_Ehdr*)stage2_elf_addr;  /* Start address of executable */
	Elf64_Phdr *s = (Elf64_Phdr*)((uint8_t*)stage2_elf_addr + elf_hdr->e_phoff);
  int ph_num = elf_hdr->e_phnum;    /* Number of program headers in ELF executable */
  int i;
  /*
   * stage2 ELF header contains 4 sections. (look at stage2/stage2.ld).
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
    printf ("Unexpected stage2 Header!\n");
    halt ();
  }
  /* After loading all sections of stage2 in the right address,
   * boot loader will transfer the control to stage2, by calling
   * the "stage2" function pointer which is assigned here. */
  *stage2 = (void (*)(struct boot_stage2_args *))((phys_addr_t)s->p_vaddr);
  /* Why Upper bound is ph_num - 1?
   * Because in stage2 ELF executable, last section is .bss which does not have any byes.
   * So we don't need to copy any bytes from this section into memory. But we do need to allocate
   * enough memory for this section when we load the executable, and we
   * also need to zero out allocated memory. That's why we put .bss section at the
   * end of executable.
   */
  for (i = 0; i < ph_num - 1; i++) {
    phys_addr_t section_src_addr = stage2_elf_addr + s->p_offset;      /* Address of section in executable */
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
  *stage2_end_addr = s->p_vaddr + s->p_memsz;    /* Shows where stage2 ends! */
}
void
boot_loader (unsigned long magic, unsigned long addr)
{
  multiboot_info_t *mbi = (multiboot_info_t *)addr;
  phys_addr_t boot_stage2_elf_addr  = 0;
  phys_addr_t boot_stage2_end_addr  = 0;
  phys_addr_t boot_aps_bin_addr     = 0;
  phys_addr_t vmm_elf_addr          = 0;
  phys_addr_t pm_elf_addr           = 0;
  phys_addr_t fs_elf_addr           = 0;
  phys_addr_t rs_elf_addr           = 0;
  phys_addr_t init_elf_addr         = 0;
  phys_addr_t initrd_elf_addr       = 0;
  size_t memory_size                = 0;
  void (*stage2)(struct boot_stage2_args *);

  clrscr ();  /* Clear the screen. */
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
    printf ("ERROR: Your processor does not support 1GB pages!\n");
    halt ();
  }
  /* Are mem_* valid? */
  if (CHECK_FLAG (mbi->flags, 0)) {
    memory_size = mbi->mem_upper;    /* Size of memory in KB */
    memory_size++;
  } else {
    printf ("ERROR: Size of memory is not known!\n");
    halt ();
  }

  if (CHECK_FLAG (mbi->flags, 3)) {
  /* stage2 should be loaded as a multi-boot module */
    multiboot_module_t *mod;
    if (mbi->mods_count != NUMBER_MODULES) {
      printf ("ERROR: Grub failed to load all needed modules. %d modules loaded, but 3 were needed!\n", mbi->mods_count);
      halt ();
    }
    mod = (multiboot_module_t*)mbi->mods_addr;
    /* ================================ */
    /* Stage2 */
    boot_stage2_elf_addr = (phys_addr_t)mod->mod_start;
    if (mod->mod_end > 0x200000) {
      printf ("ERROR: loaded module overlaps with stage2 start address\n");
      halt ();
    }
    mod++;
    /* ================================ */
    /* Loading bootaps.S */
    boot_aps_bin_addr = (phys_addr_t)mod->mod_start;
    if (mod->mod_end > 0x200000) {
      printf ("ERROR: loaded module overlaps with VMM start address\n");
      halt ();
    }
    memcpy ((void*)BOOT_APS_BASE_ADDR, (void*)boot_aps_bin_addr, BOOT_APS_LENGTH);
    mod++;
    /* ================================ */
    /* VMM is loaded here */
    vmm_elf_addr = (phys_addr_t)mod->mod_start;
    if (mod->mod_end > 0x200000) {
      printf ("ERROR: loaded module overlaps with VMM start address\n");
      halt ();
    }
    mod++;
    /* ================================ */
    /* PM is loaded here */
    pm_elf_addr = (phys_addr_t)mod->mod_start;
    if (mod->mod_end > 0x200000) {
      printf ("ERROR: loaded module overlaps with VMM start address\n");
      halt ();
    }
    mod++;
    /* ================================ */
    /* FS is loaded here */
    fs_elf_addr = (phys_addr_t)mod->mod_start;
    if (mod->mod_end > 0x200000) {
      printf ("ERROR: loaded module overlaps with VMM start address\n");
      halt ();
    }
    mod++;
    /* ================================ */
    /* Reincarnation server is loaded here */
    rs_elf_addr = (phys_addr_t)mod->mod_start;
    if (mod->mod_end > 0x200000) {
      printf ("ERROR: loaded module overlaps with VMM start address\n");
      halt ();
    }
    mod++;
    /* ================================ */
    /* init is loaded here */
    init_elf_addr = (phys_addr_t)mod->mod_start;
    if (mod->mod_end > 0x200000) {
      printf ("ERROR: loaded module overlaps with VMM start address\n");
      halt ();
    }
    mod++;
    /* ================================ */
    /* initrd is loaded here */
    initrd_elf_addr = (phys_addr_t)mod->mod_start;
    if (mod->mod_end > 0x200000) {
      printf ("ERROR: loaded module overlaps with VMM start address\n");
      halt ();
    }
    mod++;
    /* ================================ */

  } else {
    printf ("ERROR: stage2 is not loaded as multiboot!\n");
    halt ();
  }
  /*
   * Now we put each section of stage2 in the right address
   * so that we can run the stage2.
   */
  load_stage2 (boot_stage2_elf_addr, &stage2, &boot_stage2_end_addr);
  /* we pass several arguments to stage2 */
  stage2_args.sys_mem_size         = memory_size;           /* First argument: size of memory */
  stage2_args.boot_stage2_end_addr = boot_stage2_end_addr;  /* Third argument: start address of page tables */
  stage2_args.vmm_elf_addr         = vmm_elf_addr;
  stage2_args.pm_elf_addr          = pm_elf_addr;
  stage2_args.fs_elf_addr          = fs_elf_addr;
  stage2_args.rs_elf_addr          = rs_elf_addr;
  stage2_args.init_elf_addr        = init_elf_addr;
  stage2_args.initrd_elf_addr      = initrd_elf_addr;
  stage2 (&stage2_args);    /* Call stage2 */
}

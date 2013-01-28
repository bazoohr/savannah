#include <stdio.h>
#include <boot_stage2_args.h>
#include <cdef.h>

int main (void)
{
  printf ("/* Auto generated file. Don't edit! */\n");
  printf ("#ifndef __ASM_STAGE2_ARGS_CONST_H__\n");
  printf ("#define __ASM_STAGE2_ARGS_CONST_H__\n\n");

  printf ("#define SYS_MEM_SIZE %10ld\n", offsetof (struct boot_stage2_args, sys_mem_size));
  printf ("#define BOOT_STAGE2_PGTB %10ld\n", offsetof (struct boot_stage2_args, boot_stage2_page_tables));
  printf ("#define BOOT_STAGE2_END_ADDR %10ld\n", offsetof (struct boot_stage2_args, boot_stage2_end_addr));
  printf ("#define VMM_ELF_ADDR %10ld\n", offsetof (struct boot_stage2_args, vmm_elf_addr));
  printf ("#define VM_ELF_ADDR %10ld\n", offsetof (struct boot_stage2_args, vm_elf_addr));

  printf ("#endif\n");

  return 0;
}

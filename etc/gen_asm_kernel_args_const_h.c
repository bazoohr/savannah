#include <stdio.h>
#include <kernel_args.h>
#include <cdef.h>

int main (void)
{
  printf ("/* Auto generated file. Don't edit! */\n");
  printf ("#ifndef __KERNEL_ARGS_CONST_H__\n");
  printf ("#define __KERNEL_ARGS_CONST_H__\n\n");

  printf ("#define KA_MEMSZ %10ld\n", offsetof (struct kernel_args, ka_memsz));
  printf ("#define KA_KCR3 %10ld\n", offsetof (struct kernel_args, ka_kernel_cr3));
  printf ("#define KA_KEND_ADDR %10ld\n", offsetof (struct kernel_args, ka_kernel_end_addr));
  printf ("#define KA_INIT_ADDR %10ld\n", offsetof (struct kernel_args, ka_init_addr));

  printf ("#endif\n");

  return 0;
}

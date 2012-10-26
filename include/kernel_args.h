#ifndef __KERNEL_ARG_H__
#define __KERNEL_ARG_H__

#if ! defined __ASSEMBLY__

#include <types.h>

struct kernel_args {
  uint64_t  ka_memsz;
  uint64_t  ka_kernel_cr3;
  uint64_t  ka_kernel_end_addr;
  uint64_t  ka_init_addr;
};

#else 

#include <kernel_args_const.h>

#endif  /* __ASSEMBLY__ */

#endif /* __KERNEL_ARG_H__ */

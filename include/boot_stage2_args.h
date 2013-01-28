#ifndef __BOOT_STAGE2_ARG_H__
#define __BOOT_STAGE2_ARG_H__

#if ! defined __ASSEMBLY__

#include <types.h>

struct boot_stage2_args {
  uint64_t  sys_mem_size;
  uint64_t  boot_stage2_page_tables;
  uint64_t  boot_stage2_end_addr;
  uint64_t  vmm_elf_addr;
  uint64_t  vm_elf_addr;
};

#else 

#include <stage2_args_const.h>

#endif  /* __ASSEMBLY__ */

#endif /* __BOOT_STAGE2_ARG_H__ */

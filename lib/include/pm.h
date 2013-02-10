#ifndef __PM_H__
#define __PM_H__

#include <types.h>

#define FORK_IPC 1

struct fork_ipc {
  virt_addr_t cpuinfo_vaddr;
  phys_addr_t register_array_paddr;
};

int fork (void);

#endif /* __PM_H__ */

#ifndef __PM_H__
#define __PM_H__

#include <types.h>

#define FORK_IPC 1
#define EXEC_IPC 2

#define MAX_PATH 32

struct fork_ipc {
  virt_addr_t cpuinfo_vaddr;
  phys_addr_t register_array_paddr;
};

struct exec_ipc {
  char path[MAX_PATH];
  char **argv;
};

int fork (void);
void exec (char *path, char **argv);

#endif /* __PM_H__ */
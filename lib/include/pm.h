#ifndef __PM_H__
#define __PM_H__

#include <types.h>
#include <cpuinfo.h>

#define FORK_IPC    1
#define EXEC_IPC    2
#define EXIT_IPC    3
#define WAITPID_IPC 4
#define CHANNEL_IPC 5
#define CHANNEL_CLOSE_IPC 6

#define MAX_PATH 32

struct fork_ipc {
  virt_addr_t cpuinfo_vaddr;
  phys_addr_t register_array_paddr;
};

struct exec_ipc {
  char path[MAX_PATH];
  char **argv;
  phys_addr_t registers;
};

struct exit_ipc {
  int status;
};

struct waitpid_ipc {
  pid_t wait_for;
};
struct waitpid_reply {
  pid_t child_pid;
  int status;
};

struct channel_ipc {
  cpuid_t end1;
  cpuid_t end2;
};

struct channel_close_ipc {
  virt_addr_t cnl;
};

int fork (void);
void exec (char *path, char **argv);
void exit (int status);
int waitpid (int pid, int *status_buf, int options);

#endif /* __PM_H__ */

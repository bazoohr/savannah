#include <pm.h>
#include <ipc.h>
#include <string.h>
#include <memory.h>
#include <printk.h>
#include <config.h>

int
fork_internal (virt_addr_t register_array_vaddr)
{
  int result;
  struct fork_ipc fork_args;

  fork_args.cpuinfo_vaddr = (virt_addr_t)&cpuinfo;
  fork_args.register_array_paddr = virt2phys (cpuinfo, register_array_vaddr);

  msg_send (PM, FORK_IPC, &fork_args, sizeof (struct fork_ipc));
  msg_receive (PM);

  memcpy (&result, &cpuinfo->msg_input[PM].data, sizeof (int));

  return result;
}

void
exec (char *path, char **argv)
{
  struct exec_ipc exec_args;
  size_t len;

  len = strlen(path) > MAX_PATH ? MAX_PATH : strlen(path);

  strncpy(exec_args.path, path, len);
  exec_args.path[len] = '\0';
  if (argv != NULL) {
    exec_args.argv = (char **)virt2phys (cpuinfo, (virt_addr_t)argv);
  } else {
    exec_args.argv = NULL;
  }

  msg_send (PM, EXEC_IPC, &exec_args, sizeof (struct exec_ipc));
  __asm__ __volatile__ ("vmcall;");
}

#include <pm.h>
#include <ipc.h>
#include <string.h>
#include <lib_mem.h>
#include <printk.h>
#include <config.h>
#include <fs.h>
#include <misc.h>
/* ========================================================= */
int
fork_internal (virt_addr_t register_array_vaddr)
{
  int result;
  struct fork_ipc fork_args;
  int open_stdin;
  int open_stdout;

  fork_args.cpuinfo_vaddr = (virt_addr_t)&cpuinfo;
  fork_args.register_array_paddr = virt2phys (cpuinfo, register_array_vaddr);

  msg_send (PM, FORK_IPC, &fork_args, sizeof (struct fork_ipc));
  msg_receive (PM);

  memcpy (&result, &cpuinfo->msg_input[PM].data, sizeof (int));

  if (cpuinfo->cpuid != INIT) {
    open_stdin  = open ("stdin", O_RDONLY);
    open_stdout = open ("stdout", O_RDWR);

    if (open_stdin == -1 && !is_driver (cpuinfo->cpuid)) {
      printk ("fork_internal: Failed to open stdin!");
      __asm__ __volatile__ ("cli;hlt\n\t");
    }
    if (open_stdout == -1 && !is_driver (cpuinfo->cpuid)) {
      printk ("fork_internal: Failed to open stdout!");
      __asm__ __volatile__ ("cli;hlt\n\t");
    }
  }

  return result;
}
/* ========================================================= */
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
/* ========================================================= */
int
waitpid (int pid, int *status_buf, int options)
{
  struct waitpid_ipc waitpid_args;
  struct waitpid_reply *reply;

  waitpid_args.wait_for = pid;

  msg_send (PM, WAITPID_IPC, &waitpid_args, sizeof (struct waitpid_ipc));
  msg_receive (PM);

  reply = (struct waitpid_reply *)cpuinfo->msg_input[PM].data;

  *status_buf = reply->status;

  return reply->child_pid;
}

/* ========================================================= */
void exit (int status)
{
  struct exit_ipc exit_args;

  exit_args.status = status;

  msg_send (PM, EXIT_IPC, &exit_args, sizeof (struct exit_ipc));
  __asm__ __volatile__ ("vmcall\n\t");
}
/* ========================================================= */

#include <pm.h>
#include <ipc.h>
#include <string.h>
#include <memory.h>
#include <printk.h>
int
fork_internal (virt_addr_t register_array_vaddr)
{
  int result;
  struct fork_ipc fork_args;

  fork_args.cpuinfo_vaddr = (virt_addr_t)&cpuinfo;
  fork_args.register_array_paddr = virt2phys (cpuinfo, register_array_vaddr);

  msg_send (PM, FORK_IPC, &fork_args, sizeof (struct fork_ipc));
  msg_receive ();

  memcpy (&result, &cpuinfo->msg_input->data, sizeof (int));

  return result;
}

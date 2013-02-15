#include <con.h>
#include <ipc.h>
#include <string.h>
#include <config.h>

int putc(int c)
{
  struct putc_ipc tmp;
  int r;

  tmp.f = cpuinfo->cpuid;
  tmp.c = c;

  msg_send(FS, PUTC_IPC, &tmp, sizeof(struct putc_ipc));
  msg_receive(FS);

  memcpy(&r, &cpuinfo->msg_input[FS].data, sizeof(int));

  return r;
}

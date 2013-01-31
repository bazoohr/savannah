#include <ipc.h>
void
msg_send(const int to, const int data)
{
  cpuinfo->msg_output[cpuinfo->cpuid].from = cpuinfo->cpuid;
  cpuinfo->msg_output[cpuinfo->cpuid].data = data;
  cpuinfo->msg_ready[cpuinfo->cpuid] = true;
}
int
msg_receive(void)
{
  int i;
  int from = -1;

  while (1) {
    for (i = 0 ; i < 8/*get_ncpus()*/ ; i++) {
      if (cpuinfo->msg_ready[i])
        from = i;
    }
    if (from != -1)
      break;
  }

  int m = cpuinfo->msg_output[from].data;

  cpuinfo->msg_ready[from] = false;

  return m;
}



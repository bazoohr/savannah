#include <ipc.h>
#include <fs.h>
#include <string.h>

void
msg_send(const int to, const int number, const void *data, int size)
{
  int id = cpuinfo->cpuid;

  cpuinfo->msg_output[id].from = id;
  cpuinfo->msg_output[id].number = number;

  memset(&cpuinfo->msg_output[id].data, 0, MSG_DATA_SIZE);
  memcpy(&cpuinfo->msg_output[id].data, data, size);

  cpuinfo->msg_ready[id] = true;
}

struct message
msg_receive(int from)
{
  int i;

  if (from == ANY) {
    while (1) {
      for (i = 0 ; i < 8/*get_ncpus()*/ ; i++) {
        if (cpuinfo->msg_ready[i])
          from = i;
      }
      if (from != -1)
        break;
    }
  } else {
    while (1) {
      if (cpuinfo->msg_ready[from])
        break;
    }
  }

  cpuinfo->msg_ready[from] = false;

  return cpuinfo->msg_output[from];
}


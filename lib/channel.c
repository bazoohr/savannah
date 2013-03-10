#include <channel.h>
#include <string.h>
#include <panic.h>
#include <cpuinfo.h>
#include <misc.h>

#include <debug.h>

#if 0
void
cnl_send(struct channel *cnl, const void *data, const int size)
{
  if (cnl == NULL) {
    panic("No channel specified");
  }

  /*
  bool *ready = (bool*)channel;
  bool *ready_reply = (bool*)(channel + sizeof(bool));
  void *ch_data = (void*)(channel + 2 * sizeof(bool));
  */

  memcpy(cnl->data, data, size);

  /*
  *ready_reply = false;
  *ready = true;
  */
  cnl->ready_reply = false;
  cnl->ready_send  = true;
}
#endif

void __inline
cnl_send(struct channel *cnl)
{
  if (cnl == NULL) {
    panic("CPU %d: cnl_send: No channel specified", cpuinfo->cpuid);
  }

  cnl->ready_reply = false;
  cnl->ready_send  = true;
}

struct channel *
cnl_receive_any()
{
  if (!is_driver(cpuinfo->cpuid)) {
    panic ("This must be called from drivers!");
  }

  volatile virt_addr_t * volatile channels = cpuinfo->vm_info.vm_channels;

  static int i = 0;
  int from = -1;
  struct channel *from_cnl = NULL;

  while (1) {
    while (i < MAX_CHANNELS) {
      /*
       * If channels[i] is 0 then is the end of the used channes, so reset
       * i to 0.
       */
      if (channels[i] == 0) {
        i = 0;
        break;
      }

      from_cnl = (struct channel *)channels[i];
      if (from_cnl->ready_send == true) {
        from_cnl->ready_send = false;
        from = i;
        i++;
        break;
      }
      i++;
    }

    if (i >= MAX_CHANNELS) {
      i = 0;
    }

    if (from != -1)
      break;
  }

  if (from_cnl == NULL) {
    panic ("From_cnl should NEVER be null!");
  }

  return from_cnl;
}

void
cnl_receive(virt_addr_t channel)
{
  if (channel == 0) {
    panic ("Channel can not be null!");
  }

  volatile struct channel *cnl;

  while (1) {
    cnl = (struct channel*)channel;
    if (cnl->ready_reply == true) {
      cnl->ready_reply = false;
      break;
    }
  }
}

void
cnl_reply(struct channel *cnl, const int count)
{
  if (cnl == 0) {
    panic("CPU %d: cnl_reply: No channel specified", cpuinfo->cpuid);
  }

  /*
  bool *ready = (bool*)(channel + sizeof(bool));
  void *ch_data = (void*)(channel + 2 * sizeof(bool));
  */

  cnl->result = count;

  //*ready = true;
  cnl->ready_reply = true;
}

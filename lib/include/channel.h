#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include <types.h>

struct channel {
  bool ready_send;
  bool ready_reply;
  int  result;
  void *data;
};

void cnl_send(struct channel *cnl);
struct channel *cnl_receive_any();
void cnl_receive(virt_addr_t channel);
void cnl_reply(struct channel *cnl, const int count);

#endif /* __CHANNEL_H__ */

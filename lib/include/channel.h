#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include <types.h>

/*
 * Magic number if the channel is going to be closed.
 */
#define CLOSE_CHANNEL 0x87654321

struct channel {
  bool ready_send;
  bool ready_reply;
  int  result;
  char data[1];
};

void cnl_send(struct channel *cnl);
struct channel *cnl_receive_any();
void cnl_receive(virt_addr_t channel);
void cnl_reply(struct channel *cnl, const int count);

#endif /* __CHANNEL_H__ */

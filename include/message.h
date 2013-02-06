#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <types.h>

#define MSG_DATA_SIZE   64

struct message {
  cpuid_t from;
  volatile int number;
  char data[MSG_DATA_SIZE];
};

#endif /* __MESSAGE_H__ */

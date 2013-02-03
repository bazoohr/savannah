#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <types.h>

#define MSG_DATA_SIZE   64

#define ANY             -1

struct message {
  cpuid_t from;
  int number;
  char data[MSG_DATA_SIZE];
};

#endif /* __MESSAGE_H__ */

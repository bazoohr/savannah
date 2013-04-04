#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <cdef.h>
#include <types.h>

#define MSG_DATA_SIZE   (64 - 16)

struct message {
  cpuid_t from;
  volatile uint64_t number;
  char data[MSG_DATA_SIZE];
}__packed;

#endif /* __MESSAGE_H__ */

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <types.h>

struct message {
  cpuid_t from;
  uint64_t data;
};

#endif /* __MESSAGE_H__ */

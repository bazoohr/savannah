#include <frame.h>
#include <asmlink.h>
.code64
.section .text

#define ISR(name, action) \
ISRENT(name)              \
  PUSH_FRAME(%rsp);       \
  movq %rsp, %rdi;        \
  call action;            \
  POP_FRAME(%rsp);        \
  iretq

ISR(irq_handler, packet_handler)

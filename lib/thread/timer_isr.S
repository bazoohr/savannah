#include <frame.h>
#include <asmlink.h>
.code64
.section .text

#define ISR(name, action) \
ISRENT(name)              \
  PUSH_FRAME(%rsp);       \
  movq %rsp, %rdi;        \
  movq action, %rax;     \
  call *%rax;             \
  POP_FRAME(%rsp);        \
  iretq

ISR(timer_handler, scheduler)

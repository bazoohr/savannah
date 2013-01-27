#include <cpu.h>

.globl _start
_start:
  movq (CPU_INFO_PTR_ADDR), %rdi
  callq vm_main

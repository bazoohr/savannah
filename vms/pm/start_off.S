#include <cpu.h>

.globl _start
_start:
  movq (CPU_INFO_PTR_ADDR), %rax
  movq %rax, cpuinfo

  callq wait_ready
  callq vm_main

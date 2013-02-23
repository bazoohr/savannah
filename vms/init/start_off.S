#include <cpu.h>

.globl _start
_start:
  movq (CPU_INFO_PTR_ADDR), %rax
  movq %rax, cpuinfo
  /*
   * TODO:
   *     Remove This after debugging
   */
  pushq %rdi
  pushq %rsi
  callq wait_ready
  callq debug_con_init
  popq %rsi
  popq %rdi

  callq vm_main

  movq %rax, %rdi
  call exit

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
  callq debug_con_init /* Needed for panic */
  callq wait_ready
  popq %rsi
  popq %rdi

  callq vm_main

  movq %rax, %rdi
  callq exit

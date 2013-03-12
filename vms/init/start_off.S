#include <cpu.h>

.globl _start
_start:
  hlt
  /*
   * RDI contains a pointer to struct cpu_info of
   * this cpu.
   */
  movq %rdi, cpuinfo
  /*
   * TODO:
   *     Remove This after debugging
   */
  pushq %rdi
  pushq %rsi
  callq debug_con_init
  popq %rsi
  popq %rdi

  callq vm_main

  movq %rax, %rdi
  call exit

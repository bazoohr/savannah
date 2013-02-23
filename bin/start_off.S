.globl _start
_start:
  movq %rdx, cpuinfo
  /*
   * TODO:
   *     Remove This after debugging
   */
  pushq %rdi
  pushq %rsi
  callq debug_con_init
  popq %rsi
  popq %rdi
  /*
   * rdi contains argc
   * rsi contains argv
   */
  callq main

  movq %rax, %rdi
  call exit

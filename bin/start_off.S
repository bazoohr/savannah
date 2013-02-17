.globl _start
_start:
  movq %rdx, cpuinfo
  /*
   * rdi contains argc
   * rsi contains argv
   */
  callq main

  movq %rax, %rdi
  call exit

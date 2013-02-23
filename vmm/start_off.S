.globl _start
_start:
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

  call vmm_main

  /* We MUST NOT GET TO THIS POINT */
  movq %rax, %rdi
  call exit

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
  /*
   * Make sure cache is enabled
   */
  movq %cr0, %rax
  btrq $29, %rax  /* Make sure NW is unset */
  btrq $30, %rax  /* Unset CD bit */
  movq %rax, %cr0
  /* ============================== */
  call vmm_main
  /* We MUST NOT GET TO THIS POINT */
  movq %rax, %rdi
  call exit

#include <cpu.h>

.globl _start
_start:
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
  callq debug_con_init /* Needed for panic */
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
  callq vm_main

  movq %rax, %rdi
  callq exit

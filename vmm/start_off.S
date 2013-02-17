.globl _start
_start:
  movq %rdi, cpuinfo

  call vmm_main

  /* We MUST NOT GET TO THIS POINT */
  movq %rax, %rdi
  call exit

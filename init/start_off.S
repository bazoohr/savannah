  .code64
  .section .text
  .globl start_off
  .type start_off, @function
start_off:
  movq $0, %rdi
  movq $0, %rsi
  callq main

  movq %rax, %rdi
  call exit

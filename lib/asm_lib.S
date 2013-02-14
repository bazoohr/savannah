#include <asm_cpu_const.h>
.section .text
.align 0x10
.globl fork
fork:
  /*
   * All indices here are relative to the stack pointer. They start from ZERO and go on until X. We use some
   * auto-generate macros to access each element in this array. The problem that we experianced here was that
   * we subtract rsp with a maximum value which is equal to the index of last field in "struct regs".
   * The problem is that the computed address of last element in this array, is equal to the return value of fork function.
   * To avoid this problem, we first push a random value on the stack, which will be overwritten by the last field of "struct regs" (CR4)
   * and we will pop that value at the end of this function to clear the stack.
   */
  pushq $0   /* Will be overwritten by the CR4 valye */
  subq $CPU_STRUCT_SIZE, %rsp  /* Create The Buffer */
  movq %rax, CPU_REGS_RAX(%rsp)  /* Save RAX */
  /* Save RIP */
  movq (CPU_STRUCT_SIZE + 8)(%rsp), %rax
  movq %rax, CPU_REGS_RIP(%rsp)
  /* Save RSP */
  movq %rsp, %rax
  addq $(CPU_STRUCT_SIZE + 16), %rax
  movq %rax, CPU_REGS_RSP(%rsp)
  /* ================================== */
  movq %rbx, CPU_REGS_RBX(%rsp)
  movq %rcx, CPU_REGS_RCX(%rsp)
  movq %rdx, CPU_REGS_RDX(%rsp)
  movq %rbp, CPU_REGS_RBP(%rsp)
  movq %rsi, CPU_REGS_RSI(%rsp)
  movq %rdi, CPU_REGS_RDI(%rsp)
  movq %r8,  CPU_REGS_R8 (%rsp)
  movq %r9,  CPU_REGS_R9 (%rsp)
  movq %r10, CPU_REGS_R10(%rsp)
  movq %r11, CPU_REGS_R11(%rsp)
  movq %r12, CPU_REGS_R12(%rsp)
  movq %r13, CPU_REGS_R13(%rsp)
  movq %r14, CPU_REGS_R14(%rsp)
  movq %r15, CPU_REGS_R15(%rsp)
  /* ================================== */
  movq %cs, %rax
  movq %rax, CPU_REGS_CS(%rsp)
  movq %ds, %rax
  movq %rax, CPU_REGS_DS(%rsp)
  movq %es, %rax
  movq %rax, CPU_REGS_ES(%rsp)
  movq %fs, %rax
  movq %rax, CPU_REGS_FS(%rsp)
  movq %gs, %rax
  movq %rax, CPU_REGS_GS(%rsp)
  movq %ss, %rax
  movq %rax, CPU_REGS_SS(%rsp)
  /* ================================== */
  pushfq
  popq %rax
  movq %rax, CPU_REGS_RFLAGS(%rsp)
  /* ================================== */
  movq %cr0, %rax
  movq %rax, CPU_REGS_CR0(%rsp)
  /* ================================== */
  movq %cr4, %rax
  movq %rax, CPU_REGS_CR4(%rsp)
  /* ================================== */
  movq %rsp, %rdi
  callq fork_internal
  addq $CPU_STRUCT_SIZE, %rsp  /* remove The Buffer */
  /* ================================== */
  popq %rcx  /* Pop the CR4 value */
  /* ================================== */
  retq

/* ============================ */
.align 0x10
.globl reload_gdt
reload_gdt:
  cli
  lgdt (%rdi)

  movl %edx, %ds
  movl %edx, %es
  movl %edx, %fs
  movl %edx, %gs
  movl %edx, %ss

  pushq %rsi
  pushq $1f
  lretq
1:
  retq

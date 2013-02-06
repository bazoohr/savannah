/* ============================================ *
 * exception.S                                  *
 *                                              *
 * Hamid R. Bazoobandi                          *
 * Sun 22.7.2012 Amsterdam                      *
 * ============================================ */
.code64
.section .text
#include <asmlink.h>
#include <frame.h>
#include <trap.h>

#define TRAP(no)                  \
    subq $TF_RIP, %rsp;           \
    movq $(no), TF_TRAPNO(%rsp);  \
    movq $0, TF_ERR(%rsp);        \
    jmp all_traps

#define TRAP_ERR(no)              \
    subq $TF_RIP, %rsp;           \
    movq $(no), TF_TRAPNO(%rsp);  \
    jmp all_traps

TRAPENT(reserved)
  TRAP (RSVD_TRAP)
TRAPENT(divide_error)
  TRAP(IDT_DE)
TRAPENT(debug)
  TRAP(IDT_DB)
TRAPENT(nmi)
  TRAP(IDT_NMI)
TRAPENT(breakpoint)
  TRAP(IDT_BP)
TRAPENT(overflow)
  TRAP(IDT_OF)
TRAPENT(bound_range)
  TRAP(IDT_BR)
TRAPENT(invalid_opcode)
  TRAP(IDT_UD)
TRAPENT(device_not_available)
  TRAP(IDT_NM)
TRAPENT(double_fault)
  TRAP_ERR(IDT_DF)
TRAPENT(invalid_tss)
  TRAP_ERR(IDT_TS)
TRAPENT(segment_not_present)
  TRAP_ERR(IDT_NP)
TRAPENT(stack_exception)
  TRAP_ERR(IDT_SS)
TRAPENT(general_protection)
  TRAP_ERR(IDT_GP)
TRAPENT(page_fault)
  TRAP_ERR(IDT_PF)
TRAPENT(fp_exception)
  TRAP_ERR(IDT_MF)
TRAPENT(alignment_check)
  TRAP_ERR(IDT_AC)
TRAPENT(machine_check)
  TRAP_ERR(IDT_MC)
TRAPENT(xmm)
  TRAP_ERR(IDT_XF)

TRAPENT(system_call)
  TRAP (IDT_SYSCALL)

	ALIGN_TEXT
all_traps:
    movq %rdi, TF_RDI(%rsp)
    movq %rsi, TF_RSI(%rsp)
    movq %rdx, TF_RDX(%rsp)
    movq %rcx, TF_RCX(%rsp)
    movq %r8,  TF_R8 (%rsp)
    movq %r9,  TF_R9 (%rsp)
    movq %rax, TF_RAX(%rsp)
    movq %rbx, TF_RBX(%rsp)
    movq %rbp, TF_RBP(%rsp)
    movq %r10, TF_R10(%rsp)
    movq %r11, TF_R11(%rsp)
    movq %r12, TF_R12(%rsp)
    movq %r13, TF_R13(%rsp)
    movq %r14, TF_R14(%rsp)
    movq %r15, TF_R15(%rsp)

    movq %ds, %rax
    movq %rax, TF_DS(%rsp)
    movq %es, %rax
    movq %rax, TF_ES(%rsp)
    movq %fs, %rax
    movq %rax, TF_FS(%rsp)
    movq %gs, %rax
    movq %rax, TF_GS(%rsp)

    movq %rsp, %rdi
    callq trap_handler


    movq TF_DS(%rsp), %rax
    movq %rax, %ds
    movq TF_ES(%rsp), %rax
    movq %rax, %es
    movq TF_FS(%rsp), %rax
    movq %rax, %fs
    movq TF_GS(%rsp), %rax
    movq %rax, %gs

    movq TF_RDI (%rsp), %rdi
    movq TF_RSI (%rsp), %rsi
    movq TF_RDX (%rsp), %rdx
    movq TF_RCX (%rsp), %rcx
    movq TF_R8  (%rsp), %r8
    movq TF_R9  (%rsp), %r9
    movq TF_RAX (%rsp), %rax
    movq TF_RBX (%rsp), %rbx
    movq TF_RBP (%rsp), %rbp
    movq TF_R10 (%rsp), %r10
    movq TF_R11 (%rsp), %r11
    movq TF_R12 (%rsp), %r12
    movq TF_R13 (%rsp), %r13
    movq TF_R14 (%rsp), %r14
    movq TF_R15 (%rsp), %r15
    addq $TF_RIP, %rsp
    iretq

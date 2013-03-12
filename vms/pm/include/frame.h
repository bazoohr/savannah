#ifndef __FRAME_H__
#define __FRAME_H__

/*
 * Exception/Trap Stack Frame
 *
 * The ordering of this is specifically so that we can take first 6
 * the syscall arguments directly from the beginning of the frame.
 */
#ifndef __ASSEMBLY__

#include <types.h>

struct trapframe {
	register_t	tf_rdi;
	register_t	tf_rsi;
	register_t	tf_rdx;
	register_t	tf_rcx;
	register_t	tf_r8;
	register_t	tf_r9;
	register_t	tf_rax;
	register_t	tf_rbx;
	register_t	tf_rbp;
	register_t	tf_r10;
	register_t	tf_r11;
	register_t	tf_r12;
	register_t	tf_r13;
	register_t	tf_r14;
	register_t	tf_r15;
	register_t	tf_trapno;
	register_t	tf_err;
	register_t	tf_addr;
#if 0
	uint32_t	tf_trapno;
#endif
	register_t	tf_fs;
	register_t	tf_gs;
	register_t	tf_es;
	register_t	tf_ds;
#if 0
	uint32_t	tf_flags;
#endif
	/* below portion defined in hardware */
	register_t	tf_rip;
	register_t	tf_cs;
	register_t	tf_rflags;
	register_t	tf_rsp;
	register_t	tf_ss;
}__packed;
#endif

#ifdef __ASSEMBLY__

#include <asm_frame_const.h>

#define PUSH_FRAME(SP)         \
    subq $TF_RIP, %rsp;        \
    movq %rdi, TF_RDI(SP);     \
    movq %rsi, TF_RSI(SP);     \
    movq %rdx, TF_RDX(SP);     \
    movq %rcx, TF_RCX(SP);     \
    movq %r8,  TF_R8 (SP);     \
    movq %r9,  TF_R9 (SP);     \
    movq %rax, TF_RAX(SP);     \
    movq %rbx, TF_RBX(SP);     \
    movq %rbp, TF_RBP(SP);     \
    movq %r10, TF_R10(SP);     \
    movq %r11, TF_R11(SP);     \
    movq %r12, TF_R12(SP);     \
    movq %r13, TF_R13(SP);     \
    movq %r14, TF_R14(SP);     \
    movq %r15, TF_R15(SP);     \
    movq %ds, %rax;            \
    movq %rax, TF_DS(SP);      \
    movq %es, %rax;            \
    movq %rax, TF_ES(SP);      \
    movq %fs, %rax;            \
    movq %rax, TF_FS(SP);      \
    movq %gs, %rax;            \
    movq %rax, TF_GS(SP)

#define POP_FRAME(SP)          \
    movq TF_DS(SP), %rax;      \
    movq %rax, %ds;            \
    movq TF_ES(SP), %rax;      \
    movq %rax, %es;            \
    movq TF_FS(SP), %rax;      \
    movq %rax, %fs;            \
    movq TF_GS(SP), %rax;      \
    movq %rax, %gs;            \
    movq TF_RDI (SP), %rdi;    \
    movq TF_RSI (SP), %rsi;    \
    movq TF_RDX (SP), %rdx;    \
    movq TF_RCX (SP), %rcx;    \
    movq TF_R8  (SP), %r8;     \
    movq TF_R9  (SP), %r9;     \
    movq TF_RAX (SP), %rax;    \
    movq TF_RBX (SP), %rbx;    \
    movq TF_RBP (SP), %rbp;    \
    movq TF_R10 (SP), %r10;    \
    movq TF_R11 (SP), %r11;    \
    movq TF_R12 (SP), %r12;    \
    movq TF_R13 (SP), %r13;    \
    movq TF_R14 (SP), %r14;    \
    movq TF_R15 (SP), %r15;    \
    addq $TF_RIP, %rsp

#endif

#endif

	.file	"input.c"
	.text
	.p2align 4,,15
	.globl	getch
	.type	getch, @function
getch:
.LFB0:
	.cfi_startproc
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	xorl	%edi, %edi
	movl	$4, %edx
	movq	%rsp, %rsi
	call	read
	testl	%eax, %eax
	jle	.L3
	movl	(%rsp), %eax
.L2:
	addq	$24, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L3:
	.cfi_restore_state
	movl	$-1, %eax
	jmp	.L2
	.cfi_endproc
.LFE0:
	.size	getch, .-getch
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

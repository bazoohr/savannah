	.file	"panic.c"
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"PANIC %s\n"
	.text
	.p2align 4,,15
	.globl	panic
	.type	panic, @function
panic:
.LFB0:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movq	%rdi, %rdx
	movl	$4, %esi
	movl	$.LC0, %edi
	xorl	%eax, %eax
	call	cprintk
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	jmp	halt
	.cfi_endproc
.LFE0:
	.size	panic, .-panic
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

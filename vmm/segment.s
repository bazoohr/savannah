	.file	"segment.c"
	.text
	.p2align 4,,15
	.globl	create_new_gdt
	.type	create_new_gdt, @function
create_new_gdt:
.LFB0:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	$80, %edx
	xorl	%esi, %esi
	imulq	$1120, %rdi, %rbx
	subq	$48, %rsp
	.cfi_def_cfa_offset 64
	movq	$0, 16(%rsp)
	movq	$0, 32(%rsp)
	movb	$-104, 21(%rsp)
	movb	$32, 22(%rsp)
	addq	$cpus+1040, %rbx
	movb	$-110, 37(%rsp)
	movq	%rbx, %rdi
	call	memset
	leaq	16(%rbx), %rdi
	leaq	16(%rsp), %rsi
	movl	$8, %edx
	call	memcpy
	leaq	32(%rbx), %rdi
	leaq	32(%rsp), %rsi
	movl	$8, %edx
	call	memcpy
	movq	%rsp, %rdi
	movl	$32, %edx
	movl	$16, %esi
	movq	%rbx, 2(%rsp)
	movw	$79, (%rsp)
	call	reload_gdt
	addq	$48, %rsp
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	.cfi_restore 3
	ret
	.cfi_endproc
.LFE0:
	.size	create_new_gdt, .-create_new_gdt
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

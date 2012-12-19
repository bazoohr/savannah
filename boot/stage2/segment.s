	.file	"segment.c"
	.text
	.p2align 4,,15
	.globl	create_new_gdt
	.type	create_new_gdt, @function
create_new_gdt:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$56, %rsp
	.cfi_def_cfa_offset 80
	movq	$0, 16(%rsp)
	movq	$0, 32(%rsp)
	movb	$-104, 21(%rsp)
	movb	$32, 22(%rsp)
	movb	$-110, 37(%rsp)
	call	get_cpu_info
	leaq	64(%rax), %rbp
	movq	%rax, %rbx
	movl	$80, %edx
	xorl	%esi, %esi
	movq	%rbp, %rdi
	call	memset
	leaq	80(%rbx), %rdi
	leaq	16(%rsp), %rsi
	movl	$8, %edx
	call	memcpy
	leaq	96(%rbx), %rdi
	leaq	32(%rsp), %rsi
	movl	$8, %edx
	call	memcpy
	movq	%rsp, %rdi
	movl	$32, %edx
	movl	$16, %esi
	movq	%rbp, 2(%rsp)
	movw	$79, (%rsp)
	call	reload_gdt
	addq	$56, %rsp
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_restore 3
	popq	%rbp
	.cfi_def_cfa_offset 8
	.cfi_restore 6
	ret
	.cfi_endproc
.LFE0:
	.size	create_new_gdt, .-create_new_gdt
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

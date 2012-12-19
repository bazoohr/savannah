	.file	"pmap.c"
	.section	.rodata
	.align 8
.LC0:
	.string	"IO mapped memory region can not be beyond 4GB"
	.text
	.globl	map_iomem
	.type	map_iomem, @function
map_iomem:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$96, %rsp
	movl	$-20971520, -40(%rbp)
	movl	$0, -36(%rbp)
	movl	$0, -48(%rbp)
	movl	$1, -44(%rbp)
	movq	-40(%rbp), %rax
	movq	-48(%rbp), %rdx
	movq	%rdx, %rcx
	subq	%rax, %rcx
	movq	%rcx, %rax
	movq	%rax, -56(%rbp)
	movq	-56(%rbp), %rax
	movq	%rax, %rdx
	shrq	$21, %rdx
	movq	-56(%rbp), %rax
	andl	$2097151, %eax
	testq	%rax, %rax
	je	.L2
	movl	$1, %eax
	jmp	.L3
.L2:
	movl	$0, %eax
.L3:
	addq	%rdx, %rax
	movq	%rax, -64(%rbp)
	movq	kernel_pml4(%rip), %rax
	movq	%rax, -72(%rbp)
	movq	-40(%rbp), %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -16(%rbp)
	movl	$0, -28(%rbp)
	movq	-64(%rbp), %rax
	movl	%eax, -76(%rbp)
	jmp	.L4
.L7:
	movq	-16(%rbp), %rax
	shrq	$39, %rax
	andl	$511, %eax
	movl	%eax, -80(%rbp)
	movq	-16(%rbp), %rax
	shrq	$30, %rax
	andl	$511, %eax
	movl	%eax, -84(%rbp)
	movq	-16(%rbp), %rax
	shrq	$21, %rax
	andl	$511, %eax
	movl	%eax, -88(%rbp)
	movl	-80(%rbp), %eax
	cltq
	salq	$3, %rax
	addq	-72(%rbp), %rax
	movq	(%rax), %rax
	andq	$-4096, %rax
	movq	%rax, -96(%rbp)
	movl	-84(%rbp), %eax
	cltq
	salq	$3, %rax
	addq	-96(%rbp), %rax
	movq	(%rax), %rax
	andq	$-4096, %rax
	movq	%rax, -24(%rbp)
	movl	-80(%rbp), %eax
	cltq
	salq	$3, %rax
	addq	-72(%rbp), %rax
	movq	(%rax), %rax
	testq	%rax, %rax
	jne	.L5
	movl	$.LC0, %edi
	call	panic
.L5:
	movl	-84(%rbp), %eax
	cltq
	salq	$3, %rax
	addq	-96(%rbp), %rax
	movq	(%rax), %rax
	testq	%rax, %rax
	jne	.L6
	movl	$1, %edi
	call	alloc_mem_pages
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rax
	movl	$4096, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset
	movl	-84(%rbp), %eax
	cltq
	salq	$3, %rax
	addq	-96(%rbp), %rax
	movq	-24(%rbp), %rdx
	orq	$3, %rdx
	movq	%rdx, (%rax)
.L6:
	movl	-88(%rbp), %eax
	cltq
	salq	$3, %rax
	addq	-24(%rbp), %rax
	movq	-8(%rbp), %rdx
	orb	$-109, %dl
	movq	%rdx, (%rax)
	addl	$1, -28(%rbp)
	addq	$2097152, -8(%rbp)
	addq	$2097152, -16(%rbp)
.L4:
	movl	-28(%rbp), %eax
	cmpl	-76(%rbp), %eax
	jne	.L7
	leave
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	map_iomem, .-map_iomem
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

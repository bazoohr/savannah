	.file	"malloc.c"
	.text
	.p2align 4,,15
	.globl	malloc
	.type	malloc, @function
malloc:
.LFB6:
	.cfi_startproc
	movq	head(%rip), %rdx
	leaq	3(%rdi), %rcx
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rdi, %rbx
	andq	$-4, %rcx
	testq	%rdx, %rdx
	je	.L18
	.p2align 4,,10
	.p2align 3
.L16:
	movl	24(%rdx), %eax
	testl	%eax, %eax
	je	.L6
	movq	(%rdx), %rax
	cmpq	%rax, %rcx
	jbe	.L26
.L6:
	movq	8(%rdx), %rdx
	testq	%rdx, %rdx
	jne	.L16
.L18:
	leaq	40(%rbx), %rdi
	call	sbrk
	cmpq	$-1, %rax
	movq	%rax, %rdx
	je	.L15
	movq	last(%rip), %rcx
	cmpq	$0, head(%rip)
	movq	%rbx, (%rax)
	movq	$0, 8(%rax)
	movq	%rcx, 16(%rax)
	leaq	40(%rax), %rax
	movl	$0, 24(%rdx)
	movq	%rax, 32(%rdx)
	je	.L27
.L12:
	testq	%rcx, %rcx
	je	.L13
	movq	%rdx, 8(%rcx)
.L13:
	movq	%rdx, last(%rip)
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	.cfi_restore 3
	ret
	.p2align 4,,10
	.p2align 3
.L26:
	.cfi_restore_state
	subq	%rbx, %rax
	cmpq	$43, %rax
	jbe	.L7
	leaq	32(%rdx,%rbx), %rsi
	subq	$40, %rax
	movq	%rax, 8(%rsi)
	movq	8(%rdx), %rax
	leaq	8(%rsi), %rcx
	movq	%rbx, (%rdx)
	movq	%rcx, 16(%rcx)
	movl	$1, 24(%rcx)
	movq	%rax, 16(%rsi)
	movq	%rcx, 8(%rdx)
	addq	$48, %rsi
	movq	8(%rcx), %rax
	movq	%rsi, 32(%rcx)
	testq	%rax, %rax
	je	.L7
	movq	%rcx, 16(%rax)
.L7:
	movl	$0, 24(%rdx)
	leaq	40(%rdx), %rax
	popq	%rbx
	.cfi_remember_state
	.cfi_restore 3
	.cfi_def_cfa_offset 8
	ret
.L27:
	.cfi_restore_state
	movq	%rdx, head(%rip)
	jmp	.L12
.L15:
	xorl	%eax, %eax
	popq	%rbx
	.cfi_restore 3
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE6:
	.size	malloc, .-malloc
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"free: Invalid Address %x\n"
	.text
	.p2align 4,,15
	.globl	free
	.type	free, @function
free:
.LFB7:
	.cfi_startproc
	movq	head(%rip), %rax
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rdi, %rbx
	testq	%rax, %rax
	je	.L30
	cmpq	%rax, %rdi
	ja	.L40
.L30:
	movq	%rbx, %rdx
	movl	$4, %esi
	movl	$.LC0, %edi
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	.cfi_restore 3
	xorl	%eax, %eax
	jmp	cprintf
	.p2align 4,,10
	.p2align 3
.L40:
	.cfi_restore_state
	xorl	%edi, %edi
	call	sbrk
	cmpq	%rax, %rbx
	jae	.L30
	cmpq	-8(%rbx), %rbx
	jne	.L30
	movq	-24(%rbx), %rax
	leaq	-40(%rbx), %rdi
	movl	$1, -16(%rbx)
	testq	%rax, %rax
	je	.L39
	movl	24(%rax), %r8d
	testl	%r8d, %r8d
	je	.L39
	movq	8(%rax), %rdx
	testq	%rdx, %rdx
	je	.L34
	movl	24(%rdx), %esi
	testl	%esi, %esi
	je	.L28
	movq	(%rax), %rcx
	addq	(%rdx), %rcx
	movq	8(%rdx), %rdx
	addq	$40, %rcx
	testq	%rdx, %rdx
	movq	%rdx, 8(%rax)
	movq	%rcx, (%rax)
	je	.L34
	movq	%rax, 16(%rdx)
	movq	%rax, %rdi
.L36:
	movl	24(%rdx), %ecx
	testl	%ecx, %ecx
	je	.L28
	movq	(%rdi), %rax
	addq	(%rdx), %rax
	addq	$40, %rax
	movq	%rax, (%rdi)
	movq	8(%rdx), %rax
	testq	%rax, %rax
	movq	%rax, 8(%rdi)
	je	.L28
	movq	%rdi, 16(%rax)
.L28:
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	.cfi_restore 3
	ret
	.p2align 4,,10
	.p2align 3
.L39:
	.cfi_restore_state
	movq	-32(%rbx), %rdx
	testq	%rdx, %rdx
	jne	.L36
.L37:
	movq	16(%rdi), %rax
	testq	%rax, %rax
	je	.L41
	movq	$0, 8(%rax)
	movq	%rax, last(%rip)
	popq	%rbx
	.cfi_remember_state
	.cfi_restore 3
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L34:
	.cfi_restore_state
	movq	%rax, %rdi
	jmp	.L37
.L41:
	popq	%rbx
	.cfi_def_cfa_offset 8
	.cfi_restore 3
	movq	$0, head(%rip)
	movq	$0, last(%rip)
	jmp	brk
	.cfi_endproc
.LFE7:
	.size	free, .-free
	.local	head
	.comm	head,8,16
	.local	last
	.comm	last,8,16
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

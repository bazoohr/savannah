	.file	"string.c"
	.text
	.p2align 4,,15
	.globl	bcopy
	.type	bcopy, @function
bcopy:
.LFB0:
	.cfi_startproc
	testq	%rdx, %rdx
	je	.L1
	addq	$1, %rdx
	movl	$1, %r8d
	xorl	%eax, %eax
	jmp	.L3
	.p2align 4,,10
	.p2align 3
.L4:
	movq	%r8, %rax
	movq	%rcx, %r8
.L3:
	movzbl	(%rdi,%rax), %ecx
	movb	%cl, (%rsi,%rax)
	leaq	1(%r8), %rcx
	cmpq	%rdx, %rcx
	jne	.L4
.L1:
	rep
	ret
	.cfi_endproc
.LFE0:
	.size	bcopy, .-bcopy
	.p2align 4,,15
	.globl	memcpy
	.type	memcpy, @function
memcpy:
.LFB1:
	.cfi_startproc
	testq	%rdx, %rdx
	movq	%rdi, %rax
	je	.L7
	addq	$1, %rdx
	movl	$1, %r8d
	xorl	%ecx, %ecx
	jmp	.L8
	.p2align 4,,10
	.p2align 3
.L9:
	movq	%r8, %rcx
	movq	%rdi, %r8
.L8:
	movzbl	(%rsi,%rcx), %edi
	movb	%dil, (%rax,%rcx)
	leaq	1(%r8), %rdi
	cmpq	%rdx, %rdi
	jne	.L9
.L7:
	rep
	ret
	.cfi_endproc
.LFE1:
	.size	memcpy, .-memcpy
	.p2align 4,,15
	.globl	memset
	.type	memset, @function
memset:
.LFB2:
	.cfi_startproc
	testq	%rdx, %rdx
	movq	%rdi, %rax
	je	.L11
	addq	$1, %rdx
	movl	$1, %edi
	xorl	%r8d, %r8d
	jmp	.L12
	.p2align 4,,10
	.p2align 3
.L13:
	movq	%rdi, %r8
	movq	%rcx, %rdi
.L12:
	leaq	1(%rdi), %rcx
	movb	%sil, (%rax,%r8)
	cmpq	%rdx, %rcx
	jne	.L13
.L11:
	rep
	ret
	.cfi_endproc
.LFE2:
	.size	memset, .-memset
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

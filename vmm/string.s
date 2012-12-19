	.file	"string.c"
	.text
	.p2align 4,,15
	.globl	memcpy
	.type	memcpy, @function
memcpy:
.LFB0:
	.cfi_startproc
	testq	%rdx, %rdx
	movq	%rdi, %rax
	je	.L2
	movq	%rdx, %r8
	leaq	-1(%rdx), %r10
	shrq	$4, %r8
	movq	%r8, %r9
	salq	$4, %r9
	testq	%r9, %r9
	je	.L6
	leaq	16(%rsi), %rcx
	cmpq	$15, %rdx
	leaq	16(%rax), %r11
	seta	%dil
	cmpq	%rcx, %rax
	seta	%cl
	cmpq	%r11, %rsi
	seta	%r11b
	orl	%r11d, %ecx
	testb	%cl, %dil
	je	.L6
	xorl	%ecx, %ecx
	xorl	%edi, %edi
	.p2align 4,,10
	.p2align 3
.L4:
	movdqu	(%rsi,%rcx), %xmm0
	addq	$1, %rdi
	movdqu	%xmm0, (%rax,%rcx)
	addq	$16, %rcx
	cmpq	%rdi, %r8
	ja	.L4
	addq	%r9, %rsi
	subq	%r9, %r10
	cmpq	%r9, %rdx
	leaq	(%rax,%r9), %rdi
	je	.L10
.L3:
	leaq	1(%r10), %r8
	xorl	%edx, %edx
	.p2align 4,,10
	.p2align 3
.L5:
	movzbl	(%rsi,%rdx), %ecx
	movb	%cl, (%rdi,%rdx)
	addq	$1, %rdx
	cmpq	%rdx, %r8
	jne	.L5
.L2:
	rep
	ret
.L6:
	movq	%rax, %rdi
	jmp	.L3
.L10:
	.p2align 4,,8
	ret
	.cfi_endproc
.LFE0:
	.size	memcpy, .-memcpy
	.p2align 4,,15
	.globl	memset
	.type	memset, @function
memset:
.LFB1:
	.cfi_startproc
	testq	%rdx, %rdx
	movq	%rdi, %rax
	je	.L12
	negq	%rdi
	leaq	-1(%rdx), %r9
	movl	%esi, %r10d
	andl	$15, %edi
	movq	%rax, %rcx
	cmpq	%rdx, %rdi
	cmova	%rdx, %rdi
	testq	%rdi, %rdi
	je	.L13
	xorl	%r8d, %r8d
	.p2align 4,,10
	.p2align 3
.L14:
	addq	$1, %r8
	movb	%r10b, (%rcx)
	subq	$1, %r9
	addq	$1, %rcx
	cmpq	%r8, %rdi
	ja	.L14
	cmpq	%rdi, %rdx
	je	.L22
.L13:
	subq	%rdi, %rdx
	movq	%rdx, %r8
	shrq	$4, %r8
	movq	%r8, %r11
	salq	$4, %r11
	testq	%r11, %r11
	je	.L15
	movd	%esi, %xmm0
	addq	%rax, %rdi
	xorl	%esi, %esi
	punpcklbw	%xmm0, %xmm0
	punpcklwd	%xmm0, %xmm0
	pshufd	$0, %xmm0, %xmm0
	.p2align 4,,10
	.p2align 3
.L16:
	addq	$1, %rsi
	movdqa	%xmm0, (%rdi)
	addq	$16, %rdi
	cmpq	%rsi, %r8
	ja	.L16
	addq	%r11, %rcx
	subq	%r11, %r9
	cmpq	%r11, %rdx
	je	.L12
.L15:
	leaq	1(%r9), %rsi
	xorl	%edx, %edx
	.p2align 4,,10
	.p2align 3
.L17:
	movb	%r10b, (%rcx,%rdx)
	addq	$1, %rdx
	cmpq	%rdx, %rsi
	jne	.L17
.L12:
	rep
	ret
.L22:
	ret
	.cfi_endproc
.LFE1:
	.size	memset, .-memset
	.p2align 4,,15
	.globl	memcmp
	.type	memcmp, @function
memcmp:
.LFB2:
	.cfi_startproc
	xorl	%eax, %eax
	testq	%rdx, %rdx
	je	.L24
	movzbl	(%rdi), %eax
	movzbl	(%rsi), %ecx
	subl	%ecx, %eax
	jne	.L24
	subq	$1, %rdx
	xorl	%ecx, %ecx
	jmp	.L25
	.p2align 4,,10
	.p2align 3
.L26:
	movzbl	1(%rdi,%rcx), %r9d
	movzbl	1(%rsi,%rcx), %r8d
	addq	$1, %rcx
	subl	%r8d, %r9d
	jne	.L28
.L25:
	cmpq	%rdx, %rcx
	jne	.L26
	rep
	ret
	.p2align 4,,10
	.p2align 3
.L28:
	movl	%r9d, %eax
.L24:
	rep
	ret
	.cfi_endproc
.LFE2:
	.size	memcmp, .-memcmp
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

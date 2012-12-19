	.file	"printk.c"
	.text
	.p2align 4,,15
	.globl	printk
	.type	printk, @function
printk:
.LFB2:
	.cfi_startproc
	pushq	%r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	movl	$1717986919, %r12d
	pushq	%rbp
	.cfi_def_cfa_offset 32
	.cfi_offset 6, -32
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 40
	.cfi_offset 3, -40
	subq	$136, %rsp
	.cfi_def_cfa_offset 176
	leaq	176(%rsp), %rax
	movq	%rsi, 88(%rsp)
	movq	%rdx, 96(%rsp)
	movq	%rcx, 104(%rsp)
	movq	%r8, 112(%rsp)
	movq	%rax, 8(%rsp)
	leaq	80(%rsp), %rax
	movq	%r9, 120(%rsp)
	movl	$8, (%rsp)
	movq	%rax, 16(%rsp)
	.p2align 4,,10
	.p2align 3
.L39:
	movzbl	0(%rbp), %edi
	testb	%dil, %dil
	je	.L41
.L27:
	cmpb	$37, %dil
	je	.L3
	addq	$1, %rbp
	movsbl	%dil, %edi
	movl	$7, %esi
	call	kputc
	movzbl	0(%rbp), %edi
	testb	%dil, %dil
	jne	.L27
.L41:
	addq	$136, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	popq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_restore 3
	popq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_restore 6
	popq	%r12
	.cfi_def_cfa_offset 16
	.cfi_restore 12
	popq	%r13
	.cfi_def_cfa_offset 8
	.cfi_restore 13
	ret
	.p2align 4,,10
	.p2align 3
.L3:
	.cfi_restore_state
	movzbl	1(%rbp), %eax
	addq	$2, %rbp
	cmpb	$100, %al
	je	.L6
	jg	.L9
	cmpb	$99, %al
	jne	.L39
	movl	(%rsp), %edx
	cmpl	$48, %edx
	.p2align 4,,3
	jae	.L10
	movl	%edx, %eax
	addq	16(%rsp), %rax
	addl	$8, %edx
	movl	%edx, (%rsp)
.L11:
	movl	(%rax), %edi
	movl	$7, %esi
	call	kputc
	jmp	.L39
	.p2align 4,,10
	.p2align 3
.L9:
	cmpb	$115, %al
	je	.L7
	cmpb	$120, %al
	.p2align 4,,7
	jne	.L39
	movl	(%rsp), %edx
	cmpl	$48, %edx
	jae	.L12
	movl	%edx, %eax
	addq	16(%rsp), %rax
	addl	$8, %edx
	movl	%edx, (%rsp)
.L13:
	movq	(%rax), %rbx
	movabsq	$5063528411713059128, %rdx
	leaq	64(%rsp), %rdi
	movabsq	$3978425819141910832, %rax
	movq	%rdx, 40(%rsp)
	xorl	%esi, %esi
	movl	$16, %edx
	movq	%rax, 32(%rsp)
	movb	$0, 48(%rsp)
	call	memset
	movq	%rbx, %rax
	shrq	$60, %rax
	movzbl	32(%rsp,%rax), %edx
	movq	%rbx, %rax
	shrq	$56, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%dl, 79(%rsp)
	movb	%al, 78(%rsp)
	movq	%rbx, %rax
	shrq	$52, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 77(%rsp)
	movq	%rbx, %rax
	shrq	$48, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 76(%rsp)
	movq	%rbx, %rax
	shrq	$44, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 75(%rsp)
	movq	%rbx, %rax
	shrq	$40, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 74(%rsp)
	movq	%rbx, %rax
	shrq	$36, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 73(%rsp)
	movq	%rbx, %rax
	shrq	$32, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 72(%rsp)
	movq	%rbx, %rax
	shrq	$28, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 71(%rsp)
	movq	%rbx, %rax
	shrq	$24, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 70(%rsp)
	movq	%rbx, %rax
	shrq	$20, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 69(%rsp)
	movq	%rbx, %rax
	shrq	$16, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 68(%rsp)
	movq	%rbx, %rax
	shrq	$12, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 67(%rsp)
	movq	%rbx, %rax
	shrq	$8, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 66(%rsp)
	movq	%rbx, %rax
	andl	$15, %ebx
	shrq	$4, %rax
	andl	$15, %eax
	cmpb	$48, %dl
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 65(%rsp)
	movzbl	32(%rsp,%rbx), %eax
	movb	%al, 64(%rsp)
	jne	.L28
	leaq	78(%rsp), %rax
	movl	$15, %ebx
	.p2align 4,,10
	.p2align 3
.L15:
	subq	$1, %rax
	subl	$1, %ebx
	cmpb	$48, 1(%rax)
	je	.L15
	cmpl	$-1, %ebx
	je	.L42
	testl	%ebx, %ebx
	js	.L39
	.p2align 4,,10
	.p2align 3
.L33:
	movslq	%ebx, %rax
	movl	$7, %esi
	subl	$1, %ebx
	movsbl	64(%rsp,%rax), %edi
	call	kputc
	cmpl	$-1, %ebx
	jne	.L33
	jmp	.L39
	.p2align 4,,10
	.p2align 3
.L7:
	movl	(%rsp), %edx
	cmpl	$48, %edx
	jae	.L25
	movl	%edx, %eax
	addq	16(%rsp), %rax
	addl	$8, %edx
	movl	%edx, (%rsp)
.L26:
	movq	(%rax), %rdi
	movl	$7, %esi
	call	kputs
	jmp	.L39
	.p2align 4,,10
	.p2align 3
.L6:
	movl	(%rsp), %edx
	cmpl	$48, %edx
	jae	.L18
	movl	%edx, %eax
	addq	16(%rsp), %rax
	addl	$8, %edx
	movl	%edx, (%rsp)
	xorl	%edi, %edi
	movl	(%rax), %ecx
	testl	%ecx, %ecx
	js	.L43
.L20:
	xorl	%ebx, %ebx
	cmpl	$9, %ecx
	jle	.L21
	leaq	32(%rsp), %rsi
	.p2align 4,,10
	.p2align 3
.L22:
	movl	%ecx, %eax
	addl	$1, %ebx
	imull	%r12d
	movl	%ecx, %eax
	sarl	$31, %eax
	sarl	$2, %edx
	subl	%eax, %edx
	leal	(%rdx,%rdx,4), %eax
	addl	%eax, %eax
	subl	%eax, %ecx
	movb	%cl, (%rsi)
	addq	$1, %rsi
	cmpl	$9, %edx
	movl	%edx, %ecx
	jg	.L22
.L21:
	movslq	%ebx, %r13
	cmpl	$1, %edi
	movb	%cl, 32(%rsp,%r13)
	jne	.L38
	jmp	.L46
	.p2align 4,,10
	.p2align 3
.L45:
	movslq	%ebx, %r13
.L38:
	movsbl	32(%rsp,%r13), %edi
	movl	$7, %esi
	subl	$1, %ebx
	addl	$48, %edi
	call	kputc
	cmpl	$-1, %ebx
	jne	.L45
	jmp	.L39
	.p2align 4,,10
	.p2align 3
.L18:
	movq	8(%rsp), %rax
	xorl	%edi, %edi
	leaq	8(%rax), %rdx
	movq	%rdx, 8(%rsp)
	movl	(%rax), %ecx
	testl	%ecx, %ecx
	jns	.L20
.L43:
	negl	%ecx
	movb	$1, %dil
	jmp	.L20
	.p2align 4,,10
	.p2align 3
.L25:
	movq	8(%rsp), %rax
	leaq	8(%rax), %rdx
	movq	%rdx, 8(%rsp)
	jmp	.L26
	.p2align 4,,10
	.p2align 3
.L12:
	movq	8(%rsp), %rax
	leaq	8(%rax), %rdx
	movq	%rdx, 8(%rsp)
	jmp	.L13
	.p2align 4,,10
	.p2align 3
.L10:
	movq	8(%rsp), %rax
	leaq	8(%rax), %rdx
	movq	%rdx, 8(%rsp)
	jmp	.L11
.L46:
	movl	$7, %esi
	movb	$45, %dil
	call	kputc
	jmp	.L38
.L42:
	movl	$7, %esi
	movl	$48, %edi
	call	kputc
	jmp	.L39
.L28:
	movl	$15, %ebx
	jmp	.L33
	.cfi_endproc
.LFE2:
	.size	printk, .-printk
	.p2align 4,,15
	.globl	cprintk
	.type	cprintk, @function
cprintk:
.LFB3:
	.cfi_startproc
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	pushq	%r13
	.cfi_def_cfa_offset 24
	.cfi_offset 13, -24
	movl	$1717986919, %r13d
	pushq	%r12
	.cfi_def_cfa_offset 32
	.cfi_offset 12, -32
	movl	%esi, %r12d
	pushq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	movq	%rdi, %rbx
	addq	$-128, %rsp
	.cfi_def_cfa_offset 176
	leaq	176(%rsp), %rax
	movq	%rdx, 96(%rsp)
	movq	%rcx, 104(%rsp)
	movq	%r8, 112(%rsp)
	movq	%r9, 120(%rsp)
	movq	%rax, 8(%rsp)
	leaq	80(%rsp), %rax
	movl	$16, (%rsp)
	movq	%rax, 16(%rsp)
	.p2align 4,,10
	.p2align 3
.L85:
	movzbl	(%rbx), %edi
	testb	%dil, %dil
	je	.L86
.L73:
	cmpb	$37, %dil
	je	.L49
	addq	$1, %rbx
	movsbl	%dil, %edi
	movl	%r12d, %esi
	call	kputc
	movzbl	(%rbx), %edi
	testb	%dil, %dil
	jne	.L73
.L86:
	subq	$-128, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	popq	%rbx
	.cfi_def_cfa_offset 40
	.cfi_restore 3
	popq	%rbp
	.cfi_def_cfa_offset 32
	.cfi_restore 6
	popq	%r12
	.cfi_def_cfa_offset 24
	.cfi_restore 12
	popq	%r13
	.cfi_def_cfa_offset 16
	.cfi_restore 13
	popq	%r14
	.cfi_def_cfa_offset 8
	.cfi_restore 14
	ret
	.p2align 4,,10
	.p2align 3
.L49:
	.cfi_restore_state
	movzbl	1(%rbx), %eax
	addq	$2, %rbx
	cmpb	$100, %al
	je	.L52
	jg	.L55
	cmpb	$99, %al
	jne	.L85
	movl	(%rsp), %edx
	cmpl	$48, %edx
	.p2align 4,,3
	jae	.L56
	movl	%edx, %eax
	addq	16(%rsp), %rax
	addl	$8, %edx
	movl	%edx, (%rsp)
.L57:
	movl	(%rax), %edi
	movl	%r12d, %esi
	call	kputc
	jmp	.L85
	.p2align 4,,10
	.p2align 3
.L55:
	cmpb	$115, %al
	je	.L53
	cmpb	$120, %al
	.p2align 4,,7
	jne	.L85
	movl	(%rsp), %edx
	cmpl	$48, %edx
	jae	.L58
	movl	%edx, %eax
	addq	16(%rsp), %rax
	addl	$8, %edx
	movl	%edx, (%rsp)
.L59:
	movq	(%rax), %rbp
	movabsq	$5063528411713059128, %rsi
	leaq	64(%rsp), %rdi
	movabsq	$3978425819141910832, %rcx
	movq	%rsi, 40(%rsp)
	movl	$16, %edx
	xorl	%esi, %esi
	movq	%rcx, 32(%rsp)
	movb	$0, 48(%rsp)
	call	memset
	movq	%rbp, %rax
	shrq	$60, %rax
	movzbl	32(%rsp,%rax), %edx
	movq	%rbp, %rax
	shrq	$56, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%dl, 79(%rsp)
	movb	%al, 78(%rsp)
	movq	%rbp, %rax
	shrq	$52, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 77(%rsp)
	movq	%rbp, %rax
	shrq	$48, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 76(%rsp)
	movq	%rbp, %rax
	shrq	$44, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 75(%rsp)
	movq	%rbp, %rax
	shrq	$40, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 74(%rsp)
	movq	%rbp, %rax
	shrq	$36, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 73(%rsp)
	movq	%rbp, %rax
	shrq	$32, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 72(%rsp)
	movq	%rbp, %rax
	shrq	$28, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 71(%rsp)
	movq	%rbp, %rax
	shrq	$24, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 70(%rsp)
	movq	%rbp, %rax
	shrq	$20, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 69(%rsp)
	movq	%rbp, %rax
	shrq	$16, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 68(%rsp)
	movq	%rbp, %rax
	shrq	$12, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 67(%rsp)
	movq	%rbp, %rax
	shrq	$8, %rax
	andl	$15, %eax
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 66(%rsp)
	movq	%rbp, %rax
	andl	$15, %ebp
	shrq	$4, %rax
	andl	$15, %eax
	cmpb	$48, %dl
	movzbl	32(%rsp,%rax), %eax
	movb	%al, 65(%rsp)
	movzbl	32(%rsp,%rbp), %eax
	movb	%al, 64(%rsp)
	jne	.L74
	leaq	78(%rsp), %rax
	movl	$15, %ebp
	.p2align 4,,10
	.p2align 3
.L61:
	subq	$1, %rax
	subl	$1, %ebp
	cmpb	$48, 1(%rax)
	je	.L61
	cmpl	$-1, %ebp
	je	.L87
	testl	%ebp, %ebp
	js	.L85
	.p2align 4,,10
	.p2align 3
.L79:
	movslq	%ebp, %rax
	movl	%r12d, %esi
	subl	$1, %ebp
	movsbl	64(%rsp,%rax), %edi
	call	kputc
	cmpl	$-1, %ebp
	jne	.L79
	jmp	.L85
	.p2align 4,,10
	.p2align 3
.L53:
	movl	(%rsp), %edx
	cmpl	$48, %edx
	jae	.L71
	movl	%edx, %eax
	addq	16(%rsp), %rax
	addl	$8, %edx
	movl	%edx, (%rsp)
.L72:
	movq	(%rax), %rdi
	movl	%r12d, %esi
	call	kputs
	jmp	.L85
	.p2align 4,,10
	.p2align 3
.L52:
	movl	(%rsp), %edx
	cmpl	$48, %edx
	jae	.L64
	movl	%edx, %eax
	addq	16(%rsp), %rax
	addl	$8, %edx
	movl	%edx, (%rsp)
	xorl	%edi, %edi
	movl	(%rax), %ecx
	testl	%ecx, %ecx
	js	.L88
.L66:
	xorl	%ebp, %ebp
	cmpl	$9, %ecx
	jle	.L67
	leaq	32(%rsp), %rsi
	.p2align 4,,10
	.p2align 3
.L68:
	movl	%ecx, %eax
	addl	$1, %ebp
	imull	%r13d
	movl	%ecx, %eax
	sarl	$31, %eax
	sarl	$2, %edx
	subl	%eax, %edx
	leal	(%rdx,%rdx,4), %eax
	addl	%eax, %eax
	subl	%eax, %ecx
	movb	%cl, (%rsi)
	addq	$1, %rsi
	cmpl	$9, %edx
	movl	%edx, %ecx
	jg	.L68
.L67:
	movslq	%ebp, %r14
	cmpl	$1, %edi
	movb	%cl, 32(%rsp,%r14)
	jne	.L84
	jmp	.L91
	.p2align 4,,10
	.p2align 3
.L90:
	movslq	%ebp, %r14
.L84:
	movsbl	32(%rsp,%r14), %edi
	movl	%r12d, %esi
	subl	$1, %ebp
	addl	$48, %edi
	call	kputc
	cmpl	$-1, %ebp
	jne	.L90
	jmp	.L85
	.p2align 4,,10
	.p2align 3
.L64:
	movq	8(%rsp), %rax
	xorl	%edi, %edi
	leaq	8(%rax), %rdx
	movq	%rdx, 8(%rsp)
	movl	(%rax), %ecx
	testl	%ecx, %ecx
	jns	.L66
.L88:
	negl	%ecx
	movb	$1, %dil
	jmp	.L66
	.p2align 4,,10
	.p2align 3
.L71:
	movq	8(%rsp), %rax
	leaq	8(%rax), %rdx
	movq	%rdx, 8(%rsp)
	jmp	.L72
	.p2align 4,,10
	.p2align 3
.L58:
	movq	8(%rsp), %rax
	leaq	8(%rax), %rdx
	movq	%rdx, 8(%rsp)
	jmp	.L59
	.p2align 4,,10
	.p2align 3
.L56:
	movq	8(%rsp), %rax
	leaq	8(%rax), %rdx
	movq	%rdx, 8(%rsp)
	jmp	.L57
.L91:
	movl	%r12d, %esi
	movb	$45, %dil
	call	kputc
	jmp	.L84
.L87:
	movl	%r12d, %esi
	movl	$48, %edi
	call	kputc
	jmp	.L85
.L74:
	movl	$15, %ebp
	jmp	.L79
	.cfi_endproc
.LFE3:
	.size	cprintk, .-cprintk
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

	.file	"console.c"
	.text
	.p2align 4,,15
	.globl	kputc
	.type	kputc, @function
kputc:
.LFB4:
	.cfi_startproc
	cmpl	$9, %edi
	je	.L3
	cmpl	$10, %edi
	je	.L24
	movq	pos(%rip), %rdx
	movq	x(%rip), %rcx
	leaq	2(%rdx), %rax
	addq	$1, %rcx
	cmpq	$757663, %rax
	ja	.L25
.L12:
	sall	$8, %esi
	movzbl	%dil, %edi
	movq	%rax, pos(%rip)
	orl	%edi, %esi
	movq	%rcx, x(%rip)
	movw	%si, (%rdx)
.L1:
	rep
	ret
	.p2align 4,,10
	.p2align 3
.L24:
	movq	y(%rip), %rax
	movq	$0, x(%rip)
	addq	$1, %rax
	movq	%rax, y(%rip)
	leaq	(%rax,%rax,4), %rax
	salq	$5, %rax
	addq	$753664, %rax
	cmpq	$757663, %rax
	movq	%rax, pos(%rip)
	jbe	.L1
	movl	$753824, %eax
	.p2align 4,,10
	.p2align 3
.L6:
	movzbl	(%rax), %edx
	movb	%dl, -160(%rax)
	addq	$1, %rax
	cmpq	$757665, %rax
	jne	.L6
	movb	$1, %al
	.p2align 4,,10
	.p2align 3
.L7:
	movq	$0, (%rax)
	addq	$1, %rax
	cmpq	$757664, %rax
	jne	.L7
	movq	$24, y(%rip)
	movq	$757504, pos(%rip)
	ret
	.p2align 4,,10
	.p2align 3
.L3:
	movq	pos(%rip), %rax
	movq	x(%rip), %rdx
	addq	$16, %rax
	addq	$8, %rdx
	cmpq	$757663, %rax
	movq	%rax, pos(%rip)
	ja	.L26
	movb	$9, (%rax)
	movq	%rdx, x(%rip)
	ret
.L25:
	movl	$753824, %eax
	.p2align 4,,10
	.p2align 3
.L11:
	movzbl	(%rax), %edx
	movb	%dl, -160(%rax)
	addq	$1, %rax
	cmpq	$757665, %rax
	jne	.L11
	movb	$1, %al
	.p2align 4,,10
	.p2align 3
.L13:
	movq	$0, (%rax)
	addq	$1, %rax
	cmpq	$757664, %rax
	jne	.L13
	movq	$24, y(%rip)
	movl	$1, %ecx
	movb	$2, %al
	movl	$757504, %edx
	jmp	.L12
	.p2align 4,,10
	.p2align 3
.L26:
	movl	$753824, %eax
	.p2align 4,,10
	.p2align 3
.L8:
	movzbl	(%rax), %edx
	movb	%dl, -160(%rax)
	addq	$1, %rax
	cmpq	$757665, %rax
	jne	.L8
	movb	$1, %al
	.p2align 4,,10
	.p2align 3
.L10:
	movq	$0, (%rax)
	addq	$1, %rax
	cmpq	$757664, %rax
	jne	.L10
	movl	$8, %edx
	xorb	%al, %al
	movq	$24, y(%rip)
	movq	$757504, pos(%rip)
	movb	$9, (%rax)
	movq	%rdx, x(%rip)
	ret
	.cfi_endproc
.LFE4:
	.size	kputc, .-kputc
	.p2align 4,,15
	.globl	kputs
	.type	kputs, @function
kputs:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movl	%esi, %ebp
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movzbl	(%rdi), %eax
	testb	%al, %al
	je	.L27
	leaq	1(%rdi), %rbx
	.p2align 4,,10
	.p2align 3
.L29:
	movsbl	%al, %edi
	movl	%ebp, %esi
	call	kputc
	movzbl	(%rbx), %eax
	addq	$1, %rbx
	testb	%al, %al
	jne	.L29
.L27:
	popq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_restore 3
	popq	%rbp
	.cfi_def_cfa_offset 8
	.cfi_restore 6
	ret
	.cfi_endproc
.LFE5:
	.size	kputs, .-kputs
	.p2align 4,,15
	.globl	change_cursor_pos
	.type	change_cursor_pos, @function
change_cursor_pos:
.LFB6:
	.cfi_startproc
	movslq	%esi, %rsi
	movslq	%edi, %rdi
	leaq	(%rsi,%rsi,4), %rax
	movq	%rsi, y(%rip)
	movq	%rdi, x(%rip)
	salq	$5, %rax
	leaq	753664(%rax,%rdi,2), %rax
	movq	%rax, pos(%rip)
	ret
	.cfi_endproc
.LFE6:
	.size	change_cursor_pos, .-change_cursor_pos
	.p2align 4,,15
	.globl	get_cursor_pos
	.type	get_cursor_pos, @function
get_cursor_pos:
.LFB7:
	.cfi_startproc
	movq	x(%rip), %rax
	movl	%eax, (%rdi)
	movq	y(%rip), %rax
	movl	%eax, (%rsi)
	ret
	.cfi_endproc
.LFE7:
	.size	get_cursor_pos, .-get_cursor_pos
	.p2align 4,,15
	.globl	con_init
	.type	con_init, @function
con_init:
.LFB8:
	.cfi_startproc
	movq	$0, y(%rip)
	movq	$0, x(%rip)
	movq	$753664, pos(%rip)
	ret
	.cfi_endproc
.LFE8:
	.size	con_init, .-con_init
	.local	y
	.comm	y,8,16
	.local	pos
	.comm	pos,8,16
	.local	x
	.comm	x,8,16
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

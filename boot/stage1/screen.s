	.file	"screen.c"
	.text
	.p2align 4,,15
	.globl	clrscr
	.type	clrscr, @function
clrscr:
.LFB0:
	.cfi_startproc
	movl	$753664, %eax
	.p2align 4,,7
	.p2align 3
.L2:
	movb	$0, (%eax)
	addl	$1, %eax
	cmpl	$757504, %eax
	jne	.L2
	movl	$0, xpos
	movl	$0, ypos
	ret
	.cfi_endproc
.LFE0:
	.size	clrscr, .-clrscr
	.p2align 4,,15
	.globl	putchar
	.type	putchar, @function
putchar:
.LFB1:
	.cfi_startproc
	pushl	%ebx
	.cfi_def_cfa_offset 8
	.cfi_offset 3, -8
	movl	8(%esp), %eax
	movl	ypos, %edx
	cmpl	$13, %eax
	je	.L8
	cmpl	$10, %eax
	je	.L8
	movl	xpos, %ebx
	leal	(%edx,%edx,4), %ecx
	sall	$4, %ecx
	addl	%ebx, %ecx
	movb	%al, 753664(%ecx,%ecx)
	leal	1(%ebx), %eax
	cmpl	$79, %eax
	movb	$7, 753665(%ecx,%ecx)
	movl	%eax, xpos
	jle	.L5
.L8:
	addl	$1, %edx
	xorl	%eax, %eax
	cmpl	$24, %edx
	cmovl	%edx, %eax
	movl	$0, xpos
	movl	%eax, ypos
.L5:
	popl	%ebx
	.cfi_def_cfa_offset 4
	.cfi_restore 3
	ret
	.cfi_endproc
.LFE1:
	.size	putchar, .-putchar
	.local	xpos
	.comm	xpos,4,4
	.local	ypos
	.comm	ypos,4,4
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

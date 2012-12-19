	.file	"bootio.c"
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"(null)"
	.text
	.p2align 4,,15
	.globl	printf
	.type	printf, @function
printf:
.LFB1:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	pushl	%edi
	.cfi_def_cfa_offset 12
	.cfi_offset 7, -12
	pushl	%esi
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushl	%ebx
	.cfi_def_cfa_offset 20
	.cfi_offset 3, -20
	subl	$76, %esp
	.cfi_def_cfa_offset 96
	movl	96(%esp), %esi
	leal	100(%esp), %eax
	leal	44(%esp), %edx
	movl	%eax, 28(%esp)
	movl	%edx, 20(%esp)
	.p2align 4,,7
	.p2align 3
.L28:
	movsbl	(%esi), %eax
	leal	1(%esi), %ebx
	movl	%ebx, 96(%esp)
	testl	%eax, %eax
	je	.L30
.L17:
	cmpl	$37, %eax
	je	.L3
	movl	%ebx, %esi
	movl	%eax, (%esp)
	leal	1(%esi), %ebx
	call	putchar
	movsbl	(%esi), %eax
	movl	%ebx, 96(%esp)
	testl	%eax, %eax
	jne	.L17
.L30:
	addl	$76, %esp
	.cfi_remember_state
	.cfi_def_cfa_offset 20
	popl	%ebx
	.cfi_def_cfa_offset 16
	.cfi_restore 3
	popl	%esi
	.cfi_def_cfa_offset 12
	.cfi_restore 6
	popl	%edi
	.cfi_def_cfa_offset 8
	.cfi_restore 7
	popl	%ebp
	.cfi_def_cfa_offset 4
	.cfi_restore 5
	ret
	.p2align 4,,7
	.p2align 3
.L3:
	.cfi_restore_state
	movzbl	1(%esi), %eax
	addl	$2, %esi
	movl	%esi, 96(%esp)
	cmpb	$115, %al
	je	.L7
	jg	.L8
	cmpb	$100, %al
	je	.L6
.L5:
	movl	28(%esp), %edx
	movl	(%edx), %eax
	addl	$4, %edx
	movl	%edx, 28(%esp)
	movl	%eax, (%esp)
	call	putchar
	jmp	.L28
	.p2align 4,,7
	.p2align 3
.L8:
	cmpb	$117, %al
	je	.L6
	cmpb	$120, %al
	.p2align 4,,7
	jne	.L5
.L6:
	movl	28(%esp), %edx
	movsbl	%al, %ecx
	movl	(%edx), %eax
	addl	$4, %edx
	cmpl	$100, %ecx
	movl	%edx, 28(%esp)
	jne	.L9
	movl	%eax, %edx
	shrl	$31, %edx
	testb	%dl, %dl
	je	.L9
	movb	$45, 44(%esp)
	negl	%eax
	movl	$10, 24(%esp)
	leal	45(%esp), %ebx
.L10:
	movl	%ebx, %ecx
	jmp	.L13
	.p2align 4,,7
	.p2align 3
.L19:
	movl	%edi, %ecx
.L13:
	xorl	%edx, %edx
	divl	24(%esp)
	movl	%edx, %ebp
	leal	48(%ebp), %edi
	addl	$87, %ebp
	cmpl	$9, %edx
	cmovg	%ebp, %edi
	testl	%eax, %eax
	movl	%edi, %edx
	movb	%dl, (%ecx)
	leal	1(%ecx), %edi
	jne	.L19
	cmpl	%ecx, %ebx
	movb	$0, 1(%ecx)
	leal	44(%esp), %edi
	jae	.L14
	.p2align 4,,7
	.p2align 3
.L24:
	movzbl	(%ebx), %eax
	movzbl	(%ecx), %edx
	movb	%dl, (%ebx)
	addl	$1, %ebx
	movb	%al, (%ecx)
	subl	$1, %ecx
	cmpl	%ecx, %ebx
	jb	.L24
	leal	44(%esp), %edi
.L14:
	movzbl	(%edi), %eax
	testb	%al, %al
	je	.L28
	.p2align 4,,7
	.p2align 3
.L23:
	movsbl	%al, %eax
	addl	$1, %edi
	movl	%eax, (%esp)
	call	putchar
	movzbl	(%edi), %eax
	testb	%al, %al
	jne	.L23
	jmp	.L28
	.p2align 4,,7
	.p2align 3
.L9:
	movl	20(%esp), %ebx
	cmpl	$120, %ecx
	movl	$16, %edx
	movl	$10, 24(%esp)
	cmovne	24(%esp), %edx
	cmove	%ebx, %ebx
	movl	%edx, 24(%esp)
	jmp	.L10
	.p2align 4,,7
	.p2align 3
.L7:
	movl	28(%esp), %eax
	movl	$.LC0, %edx
	movl	(%eax), %edi
	addl	$4, %eax
	movl	%eax, 28(%esp)
	testl	%edi, %edi
	cmove	%edx, %edi
	jmp	.L14
	.cfi_endproc
.LFE1:
	.size	printf, .-printf
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

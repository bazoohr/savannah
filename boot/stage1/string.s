	.file	"string.c"
	.text
	.p2align 4,,15
	.globl	memcpy
	.type	memcpy, @function
memcpy:
.LFB0:
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
	subl	$16, %esp
	.cfi_def_cfa_offset 36
	movl	44(%esp), %edi
	movl	36(%esp), %eax
	movl	40(%esp), %ebx
	testl	%edi, %edi
	je	.L2
	movl	%edi, %ecx
	shrl	$2, %ecx
	movl	%ecx, %ebp
	sall	$2, %ebp
	leal	-1(%edi), %edx
	testl	%ebp, %ebp
	movl	%edx, 8(%esp)
	movl	%ecx, 4(%esp)
	je	.L6
	cmpl	$9, %edi
	movl	%ebx, %edx
	seta	3(%esp)
	orl	%eax, %edx
	andl	$3, %edx
	sete	%dl
	andb	%dl, 3(%esp)
	leal	4(%ebx), %edx
	cmpl	%edx, %eax
	leal	4(%eax), %ecx
	seta	%dl
	cmpl	%ecx, %ebx
	seta	%cl
	movl	%ecx, %esi
	orl	%esi, %edx
	testb	%dl, 3(%esp)
	je	.L6
	movl	4(%esp), %esi
	xorl	%edx, %edx
	.p2align 4,,7
	.p2align 3
.L4:
	movl	(%ebx,%edx,4), %ecx
	movl	%ecx, (%eax,%edx,4)
	addl	$1, %edx
	cmpl	%edx, %esi
	ja	.L4
	addl	%ebp, %ebx
	subl	%ebp, 8(%esp)
	cmpl	%ebp, %edi
	leal	(%eax,%ebp), %ecx
	je	.L2
.L3:
	movl	8(%esp), %esi
	xorl	%edx, %edx
	leal	1(%esi), %edi
	movl	%ecx, %esi
	.p2align 4,,7
	.p2align 3
.L5:
	movzbl	(%ebx,%edx), %ecx
	movb	%cl, (%esi,%edx)
	addl	$1, %edx
	cmpl	%edx, %edi
	jne	.L5
.L2:
	addl	$16, %esp
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
.L6:
	.cfi_restore_state
	movl	%eax, %ecx
	jmp	.L3
	.cfi_endproc
.LFE0:
	.size	memcpy, .-memcpy
	.p2align 4,,15
	.globl	bcopy
	.type	bcopy, @function
bcopy:
.LFB1:
	.cfi_startproc
	pushl	%edi
	.cfi_def_cfa_offset 8
	.cfi_offset 7, -8
	pushl	%esi
	.cfi_def_cfa_offset 12
	.cfi_offset 6, -12
	pushl	%ebx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	24(%esp), %ebx
	movl	16(%esp), %esi
	movl	20(%esp), %edi
	testl	%ebx, %ebx
	je	.L10
	xorl	%edx, %edx
	xorl	%eax, %eax
	.p2align 4,,7
	.p2align 3
.L12:
	movzbl	(%esi,%edx), %ecx
	addl	$1, %eax
	cmpl	%ebx, %eax
	movb	%cl, (%edi,%edx)
	movl	%eax, %edx
	jne	.L12
.L10:
	popl	%ebx
	.cfi_def_cfa_offset 12
	.cfi_restore 3
	popl	%esi
	.cfi_def_cfa_offset 8
	.cfi_restore 6
	popl	%edi
	.cfi_def_cfa_offset 4
	.cfi_restore 7
	ret
	.cfi_endproc
.LFE1:
	.size	bcopy, .-bcopy
	.p2align 4,,15
	.globl	memset
	.type	memset, @function
memset:
.LFB2:
	.cfi_startproc
	pushl	%edi
	.cfi_def_cfa_offset 8
	.cfi_offset 7, -8
	pushl	%esi
	.cfi_def_cfa_offset 12
	.cfi_offset 6, -12
	pushl	%ebx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	24(%esp), %edi
	movl	16(%esp), %eax
	testl	%edi, %edi
	je	.L15
	movzbl	20(%esp), %esi
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	.p2align 4,,7
	.p2align 3
.L16:
	addl	$1, %edx
	movl	%esi, %ebx
	cmpl	%edi, %edx
	movb	%bl, (%eax,%ecx)
	movl	%edx, %ecx
	jne	.L16
.L15:
	popl	%ebx
	.cfi_def_cfa_offset 12
	.cfi_restore 3
	popl	%esi
	.cfi_def_cfa_offset 8
	.cfi_restore 6
	popl	%edi
	.cfi_def_cfa_offset 4
	.cfi_restore 7
	ret
	.cfi_endproc
.LFE2:
	.size	memset, .-memset
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

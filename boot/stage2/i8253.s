	.file	"i8253.c"
	.text
	.p2align 4,,15
	.globl	i8253_delay
	.type	i8253_delay, @function
i8253_delay:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	testq	%rdi, %rdi
	je	.L1
	xorl	%ebx, %ebx
	.p2align 4,,10
	.p2align 3
.L4:
	movl	$97, %edi
	call	inb
	movl	$12, %edi
	movl	$97, %esi
	andl	%eax, %edi
	orl	$1, %edi
	call	outb
	movl	$67, %esi
	movl	$176, %edi
	call	outb
	movl	$66, %esi
	movl	$169, %edi
	call	outb
	movl	$66, %esi
	movl	$4, %edi
	call	outb
	.p2align 4,,10
	.p2align 3
.L3:
	movl	$97, %edi
	call	inb
	testb	$32, %al
	je	.L3
	movl	$97, %edi
	addq	$1, %rbx
	call	inb
	movl	$12, %edi
	movl	$97, %esi
	andl	%eax, %edi
	call	outb
	cmpq	%rbp, %rbx
	jne	.L4
.L1:
	addq	$8, %rsp
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
	.size	i8253_delay, .-i8253_delay
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

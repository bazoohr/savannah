	.file	"ioapic.c"
	.text
	.p2align 4,,15
	.globl	ioapic_read
	.type	ioapic_read, @function
ioapic_read:
.LFB0:
	.cfi_startproc
	movq	ioapic(%rip), %rax
	movl	%edi, (%rax)
	movl	16(%rax), %eax
	ret
	.cfi_endproc
.LFE0:
	.size	ioapic_read, .-ioapic_read
	.p2align 4,,15
	.globl	ioapic_enable
	.type	ioapic_enable, @function
ioapic_enable:
.LFB2:
	.cfi_startproc
	movq	ioapic(%rip), %rax
	leal	32(%rdi), %edx
	leal	16(%rdi,%rdi), %edi
	sall	$24, %esi
	movl	%edi, (%rax)
	addl	$1, %edi
	movl	%edx, 16(%rax)
	movl	%edi, (%rax)
	movl	%esi, 16(%rax)
	ret
	.cfi_endproc
.LFE2:
	.size	ioapic_enable, .-ioapic_enable
	.p2align 4,,15
	.globl	ioapic_disable
	.type	ioapic_disable, @function
ioapic_disable:
.LFB3:
	.cfi_startproc
	testl	%edi, %edi
	je	.L7
	movq	ioapic(%rip), %rax
	leal	32(%rdi), %edx
	leal	16(%rdi,%rdi), %edi
	orl	$65536, %edx
	movl	%edi, (%rax)
	addl	$1, %edi
	movl	%edx, 16(%rax)
	movl	%edi, (%rax)
	movl	$0, 16(%rax)
	ret
	.p2align 4,,10
	.p2align 3
.L7:
	movq	ioapic(%rip), %rax
	movb	$16, %dil
	addl	$1, %edi
	movl	%edi, (%rax)
	movl	$0, 16(%rax)
	ret
	.cfi_endproc
.LFE3:
	.size	ioapic_disable, .-ioapic_disable
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"ioapicva is NOT set"
	.text
	.p2align 4,,15
	.globl	ioapic_init
	.type	ioapic_init, @function
ioapic_init:
.LFB4:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movq	ioapicva(%rip), %rax
	testq	%rax, %rax
	je	.L12
.L9:
	movq	%rax, ioapic(%rip)
	movl	$1, (%rax)
	movl	$16, %ecx
	movl	16(%rax), %edi
	xorl	%edx, %edx
	shrl	$16, %edi
	andl	$255, %edi
	.p2align 4,,10
	.p2align 3
.L10:
	leal	32(%rdx), %esi
	movl	%ecx, (%rax)
	addl	$1, %edx
	orl	$65536, %esi
	movl	%esi, 16(%rax)
	leal	1(%rcx), %esi
	addl	$2, %ecx
	cmpl	%edx, %edi
	movl	%esi, (%rax)
	movl	$0, 16(%rax)
	jge	.L10
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
.L12:
	.cfi_restore_state
	movl	$.LC0, %edi
	call	panic
	movq	ioapicva(%rip), %rax
	jmp	.L9
	.cfi_endproc
.LFE4:
	.size	ioapic_init, .-ioapic_init
	.comm	ioapic,8,16
	.comm	ioapicid,4,4
	.comm	ioapicva,8,16
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

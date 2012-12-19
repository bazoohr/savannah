	.file	"i8259.c"
	.text
	.p2align 4,,15
	.globl	i8259_init
	.type	i8259_init, @function
i8259_init:
.LFB1:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	$32, %esi
	movl	$17, %edi
	movw	$255, i8259_mask(%rip)
	call	outb
	movl	$160, %esi
	movl	$17, %edi
	call	outb
	movl	$33, %esi
	movl	$32, %edi
	call	outb
	movl	$161, %esi
	movl	$40, %edi
	call	outb
	movl	$33, %esi
	movl	$4, %edi
	call	outb
	movl	$161, %esi
	movl	$2, %edi
	call	outb
	movl	$33, %esi
	movl	$1, %edi
	call	outb
	movl	$161, %esi
	movl	$1, %edi
	call	outb
	movzbl	i8259_mask(%rip), %edi
	movl	$33, %esi
	call	outb
	movzbl	i8259_mask+1(%rip), %edi
	movl	$161, %esi
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	jmp	outb
	.cfi_endproc
.LFE1:
	.size	i8259_init, .-i8259_init
	.p2align 4,,15
	.globl	i8259_eoi
	.type	i8259_eoi, @function
i8259_eoi:
.LFB2:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	$32, %esi
	movl	$32, %edi
	call	outb
	movl	$160, %esi
	movl	$32, %edi
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	jmp	outb
	.cfi_endproc
.LFE2:
	.size	i8259_eoi, .-i8259_eoi
	.p2align 4,,15
	.globl	i8259_enable
	.type	i8259_enable, @function
i8259_enable:
.LFB3:
	.cfi_startproc
	movzwl	i8259_mask(%rip), %eax
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	%edi, %ecx
	movl	$-2, %ebx
	roll	%cl, %ebx
	andl	%eax, %ebx
	cmpw	%bx, %ax
	je	.L3
	movzbl	%bl, %edi
	movl	$33, %esi
	call	outb
	movzbl	%bh, %edi
	movl	$161, %esi
	call	outb
	movw	%bx, i8259_mask(%rip)
.L3:
	popq	%rbx
	.cfi_def_cfa_offset 8
	.cfi_restore 3
	ret
	.cfi_endproc
.LFE3:
	.size	i8259_enable, .-i8259_enable
	.p2align 4,,15
	.globl	i8259_disable
	.type	i8259_disable, @function
i8259_disable:
.LFB4:
	.cfi_startproc
	movzwl	i8259_mask(%rip), %eax
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	%edi, %ecx
	movl	$1, %ebx
	sall	%cl, %ebx
	orl	%eax, %ebx
	cmpw	%bx, %ax
	je	.L6
	movzbl	%bl, %edi
	movl	$33, %esi
	call	outb
	movzbl	%bh, %edi
	movl	$161, %esi
	call	outb
	movw	%bx, i8259_mask(%rip)
.L6:
	popq	%rbx
	.cfi_def_cfa_offset 8
	.cfi_restore 3
	ret
	.cfi_endproc
.LFE4:
	.size	i8259_disable, .-i8259_disable
	.local	i8259_mask
	.comm	i8259_mask,2,16
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

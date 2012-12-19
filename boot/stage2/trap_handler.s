	.file	"trap_handler.c"
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"page fault addr = %x\n"
.LC1:
	.string	"trap number %d\n"
.LC2:
	.string	"cs %x\n"
.LC3:
	.string	"rbp %x\n"
.LC4:
	.string	"rsp %x\n"
.LC5:
	.string	"rip %x\n"
.LC6:
	.string	"trap_no %x\n"
.LC7:
	.string	"error %x\n"
	.text
	.p2align 4,,15
	.globl	trap_handler
	.type	trap_handler, @function
trap_handler:
.LFB1:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	120(%rdi), %rsi
	movq	%rdi, %rbx
	cmpq	$14, %rsi
	je	.L5
.L2:
	cmpq	$128, %rsi
	je	.L1
	movl	$.LC1, %edi
	xorl	%eax, %eax
	call	printk
	movq	184(%rbx), %rsi
	movl	$.LC2, %edi
	xorl	%eax, %eax
	call	printk
	movq	64(%rbx), %rsi
	movl	$.LC3, %edi
	xorl	%eax, %eax
	call	printk
	movq	200(%rbx), %rsi
	movl	$.LC4, %edi
	xorl	%eax, %eax
	call	printk
	movq	176(%rbx), %rsi
	movl	$.LC5, %edi
	xorl	%eax, %eax
	call	printk
	movq	120(%rbx), %rsi
	movl	$.LC6, %edi
	xorl	%eax, %eax
	call	printk
	movq	128(%rbx), %rsi
	movl	$.LC7, %edi
	xorl	%eax, %eax
	call	printk
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	.cfi_restore 3
	jmp	halt
	.p2align 4,,10
	.p2align 3
.L1:
	.cfi_restore_state
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	.cfi_restore 3
	ret
	.p2align 4,,10
	.p2align 3
.L5:
	.cfi_restore_state
	call	rcr2
	movl	$.LC0, %edi
	movq	%rax, %rsi
	xorl	%eax, %eax
	call	printk
	movq	120(%rbx), %rsi
	jmp	.L2
	.cfi_endproc
.LFE1:
	.size	trap_handler, .-trap_handler
	.comm	__packed,216,32
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

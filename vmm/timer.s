	.file	"timer.c"
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"ticks = %d\n"
	.text
	.p2align 4,,15
	.globl	do_timer
	.type	do_timer, @function
do_timer:
.LFB0:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movq	ticks(%rip), %rdx
	movl	$9, %esi
	movl	$.LC0, %edi
	leaq	1(%rdx), %rax
	movq	%rax, ticks(%rip)
	xorl	%eax, %eax
	call	cprintk
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	jmp	lapic_eoi
	.cfi_endproc
.LFE0:
	.size	do_timer, .-do_timer
	.p2align 4,,15
	.globl	timer_delay
	.type	timer_delay, @function
timer_delay:
.LFB1:
	.cfi_startproc
	jmp	i8253_delay
	.cfi_endproc
.LFE1:
	.size	timer_delay, .-timer_delay
	.globl	ticks
	.bss
	.align 8
	.type	ticks, @object
	.size	ticks, 8
ticks:
	.zero	8
	.comm	__packed,216,32
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

	.file	"thread.c"
	.text
	.p2align 4,,15
	.type	thread_template, @function
thread_template:
.LFB4:
	.cfi_startproc
	movq	%rdi, %rax
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movq	8(%rdi), %rdi
	call	*(%rax)
	movq	%rax, %rdi
	call	unclone
	xorl	%eax, %eax
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE4:
	.size	thread_template, .-thread_template
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC0:
	.string	"thread_create: maximum number of threads have been created!\n"
	.align 8
.LC1:
	.string	"thread_create: failed to alloc memory for thread's stack!\n"
	.text
	.p2align 4,,15
	.globl	thread_create
	.type	thread_create, @function
thread_create:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	movl	first_time(%rip), %eax
	testl	%eax, %eax
	je	.L3
	movl	$thread_info_table+24, %eax
	.p2align 4,,10
	.p2align 3
.L4:
	movl	$1, (%rax)
	addq	$32, %rax
	cmpq	$thread_info_table+8216, %rax
	jne	.L4
	movl	$0, first_time(%rip)
.L3:
	movl	$thread_info_table+24, %eax
	xorl	%edx, %edx
	jmp	.L8
	.p2align 4,,10
	.p2align 3
.L5:
	addl	$1, %edx
	addq	$32, %rax
	cmpl	$256, %edx
	je	.L16
.L8:
	cmpl	$1, (%rax)
	jne	.L5
	movslq	%edx, %rdx
	salq	$5, %rdx
	leaq	thread_info_table(%rdx), %rbx
	movq	%rdi, thread_info_table(%rdx)
	movl	$12288, %edi
	movl	$0, thread_info_table+24(%rdx)
	movq	%rsi, 8(%rbx)
	call	malloc
	testq	%rax, %rax
	movq	%rax, 16(%rbx)
	je	.L17
	leaq	4095(%rax), %rdx
	movl	$4096, %ecx
	movq	%rbx, %rsi
	movl	$thread_template, %edi
	andq	$-4096, %rdx
	call	clone
	testl	%eax, %eax
	movl	%eax, %ebp
	js	.L18
.L10:
	movl	$-1, %eax
	testl	%ebp, %ebp
	movl	%ebp, 28(%rbx)
	cmovns	%ebp, %eax
.L9:
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_restore 3
	popq	%rbp
	.cfi_def_cfa_offset 8
	.cfi_restore 6
	ret
.L18:
	.cfi_restore_state
	movq	16(%rbx), %rdi
	call	free
	movl	$1, 24(%rbx)
	jmp	.L10
.L16:
	xorl	%eax, %eax
	movl	$4, %esi
	movl	$.LC0, %edi
	call	cprintf
	movl	$-1, %eax
	jmp	.L9
.L17:
	xorl	%eax, %eax
	movl	$4, %esi
	movl	$.LC1, %edi
	call	cprintf
	movl	$-1, %eax
	jmp	.L9
	.cfi_endproc
.LFE5:
	.size	thread_create, .-thread_create
	.section	.rodata.str1.8
	.align 8
.LC2:
	.string	"thread_join: didn't find a thread with id = %d\n"
	.text
	.p2align 4,,15
	.globl	thread_join
	.type	thread_join, @function
thread_join:
.LFB6:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	$thread_info_table+28, %eax
	xorl	%ebx, %ebx
	jmp	.L22
	.p2align 4,,10
	.p2align 3
.L20:
	addl	$1, %ebx
	addq	$32, %rax
	cmpl	$256, %ebx
	je	.L24
.L22:
	cmpl	(%rax), %edi
	jne	.L20
	movslq	%ebx, %rbx
	xorl	%esi, %esi
	salq	$5, %rbx
	call	waitpid
	movq	thread_info_table+16(%rbx), %rdi
	call	free
	movl	$1, thread_info_table+24(%rbx)
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	.cfi_restore 3
	ret
	.p2align 4,,10
	.p2align 3
.L24:
	.cfi_restore_state
	popq	%rbx
	.cfi_def_cfa_offset 8
	.cfi_restore 3
	movl	%edi, %edx
	movl	$4, %esi
	movl	$.LC2, %edi
	xorl	%eax, %eax
	jmp	cprintf
	.cfi_endproc
.LFE6:
	.size	thread_join, .-thread_join
	.comm	thread_info_table,8192,32
	.data
	.align 16
	.type	first_time, @object
	.size	first_time, 4
first_time:
	.long	1
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

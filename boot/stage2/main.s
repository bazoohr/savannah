	.file	"main.c"
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC0:
	.string	"mep_memory: memory start region can not be greater than end address"
	.align 8
.LC1:
	.string	"Trying to map addresses into empty page tables!"
	.text
	.p2align 4,,15
	.globl	map_memory
	.type	map_memory, @function
map_memory:
.LFB0:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	movq	%rsi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	movq	%rdi, %rbx
	subq	$24, %rsp
	.cfi_def_cfa_offset 80
	movq	(%rcx), %rax
	movq	%rcx, 8(%rsp)
	addq	$4095, %rax
	andq	$-4096, %rax
	cmpq	%rdx, %rsi
	movq	%rax, (%rcx)
	ja	.L11
.L2:
	cmpq	%rdx, %rbp
	je	.L1
	subq	%rbp, %rdx
	xorl	%eax, %eax
	testl	$2097151, %edx
	setne	%al
	shrq	$21, %rdx
	addq	%rdx, %rax
	testl	%eax, %eax
	je	.L1
	subl	$1, %eax
	salq	$21, %rax
	leaq	2097152(%rbp,%rax), %r15
	jmp	.L7
	.p2align 4,,10
	.p2align 3
.L4:
	andq	$-4096, %r12
	testq	%rax, %rax
	je	.L12
.L6:
	movq	%rbp, %rax
	addq	$2097152, %rbp
	movslq	%r14d, %r14
	orb	$-109, %al
	cmpq	%r15, %rbp
	movq	%rax, (%r12,%r14,8)
	je	.L1
.L7:
	movq	%rbp, %rax
	movq	%rbp, %r14
	shrq	$39, %rax
	shrq	$21, %r14
	andl	$511, %eax
	andl	$511, %r14d
	movq	(%rbx,%rax,8), %rdx
	movq	%rbp, %rax
	shrq	$27, %rax
	andl	$4088, %eax
	movq	%rdx, %r13
	andq	$-4096, %r13
	addq	%rax, %r13
	testq	%rdx, %rdx
	movq	0(%r13), %r12
	movq	%r12, %rax
	jne	.L4
	movl	$.LC1, %edi
	andq	$-4096, %r12
	call	panic
	movq	0(%r13), %rax
	testq	%rax, %rax
	jne	.L6
.L12:
	movq	8(%rsp), %rcx
	movl	$4096, %edx
	xorl	%esi, %esi
	movq	(%rcx), %r12
	leaq	4096(%r12), %rax
	movq	%r12, %rdi
	movq	%rax, (%rcx)
	call	memset
	movq	%r12, %rax
	orq	$3, %rax
	movq	%rax, 0(%r13)
	jmp	.L6
	.p2align 4,,10
	.p2align 3
.L1:
	addq	$24, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_restore 3
	popq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_restore 6
	popq	%r12
	.cfi_def_cfa_offset 32
	.cfi_restore 12
	popq	%r13
	.cfi_def_cfa_offset 24
	.cfi_restore 13
	popq	%r14
	.cfi_def_cfa_offset 16
	.cfi_restore 14
	popq	%r15
	.cfi_def_cfa_offset 8
	.cfi_restore 15
	ret
	.p2align 4,,10
	.p2align 3
.L11:
	.cfi_restore_state
	movl	$.LC0, %edi
	movq	%rdx, (%rsp)
	call	panic
	movq	(%rsp), %rdx
	jmp	.L2
	.cfi_endproc
.LFE0:
	.size	map_memory, .-map_memory
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC2:
	.string	"stage2_end_addr = %x\n"
	.section	.rodata.str1.8
	.align 8
.LC3:
	.string	"ELF executable contains %d sections!\n"
	.section	.rodata.str1.1
.LC4:
	.string	"Unexpected VMM Header!\n"
.LC5:
	.string	"vmm is bigger than expected!"
.LC6:
	.string	"stack %x "
.LC7:
	.string	"vmm_start %x\n"
	.text
	.p2align 4,,15
	.globl	kmain
	.type	kmain, @function
kmain:
.LFB2:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$40, %rsp
	.cfi_def_cfa_offset 96
	movq	16(%rdi), %rax
	movq	8(%rdi), %rbp
	movq	24(%rdi), %rbx
	movq	%rax, 24(%rsp)
	xorl	%eax, %eax
	call	con_init
	movq	24(%rsp), %rdx
	xorl	%eax, %eax
	movl	$6, %esi
	movl	$.LC2, %edi
	call	cprintk
	leaq	24(%rsp), %rcx
	movabsq	$4294967296, %rdx
	movl	$4273995776, %esi
	movq	%rbp, %rdi
	call	map_memory
	movq	24(%rsp), %rdx
	xorl	%eax, %eax
	movl	$6, %esi
	movl	$.LC2, %edi
	call	cprintk
	call	mp_init
	call	interrupt_init
	.p2align 4,,5
	call	pic_init
	.p2align 4,,5
	call	ioapic_init
	.p2align 4,,5
	call	lapic_init
	.p2align 4,,5
	call	kbd_init
	movq	24(%rsp), %r15
	movq	$0, (%rsp)
	call	get_ncpus
	addq	$2097151, %r15
	andq	$-2097152, %r15
	cmpq	%rax, (%rsp)
	jae	.L26
	.p2align 4,,10
	.p2align 3
.L21:
	movq	(%rsp), %rdi
	movq	%rbx, %rbp
	call	get_cpu_info
	movzwl	56(%rbx), %r13d
	addq	32(%rbx), %rbp
	movq	%rax, %r14
	movq	%r15, 24(%rax)
	cmpl	$4, %r13d
	je	.L15
	xorl	%eax, %eax
	movl	%r13d, %edx
	movl	$4, %esi
	movl	$.LC3, %edi
	subl	$1, %r13d
	call	cprintk
	movl	$.LC4, %edi
	call	panic
	movq	16(%rbp), %rax
	testl	%r13d, %r13d
	movq	%rax, 40(%r14)
	jle	.L17
.L16:
	leaq	-1073741824(%r15), %rax
	xorl	%r12d, %r12d
	movq	%rax, 8(%rsp)
	jmp	.L19
	.p2align 4,,10
	.p2align 3
.L18:
	addl	$1, %r12d
	addq	$56, %rbp
	cmpl	%r13d, %r12d
	jge	.L27
.L19:
	movq	40(%rbp), %rdx
	movq	8(%rbp), %rsi
	movq	16(%rbp), %rdi
	testq	%rdx, %rdx
	je	.L18
	addq	8(%rsp), %rdi
	addq	%rbx, %rsi
	addl	$1, %r12d
	addq	$56, %rbp
	call	memcpy
	cmpl	%r13d, %r12d
	jl	.L19
.L27:
	movq	40(%r14), %rax
.L17:
	movq	16(%rbp), %rdx
	addq	40(%rbp), %rdx
	addq	$15, %rdx
	andq	$-16, %rdx
	addq	$4096, %rdx
	movq	%rdx, %rbp
	movq	%rdx, 8(%r14)
	subq	%rax, %rbp
	cmpq	$2097152, %rbp
	ja	.L28
.L20:
	addq	24(%r14), %rbp
	xorl	%eax, %eax
	movl	$14, %esi
	movl	$.LC6, %edi
	addq	$2097152, %r15
	movq	%rbp, 32(%r14)
	call	cprintk
	movq	24(%r14), %rdx
	xorl	%eax, %eax
	movl	$14, %esi
	movl	$.LC7, %edi
	call	cprintk
	addq	$1, (%rsp)
	call	get_ncpus
	cmpq	%rax, (%rsp)
	jb	.L21
.L26:
	xorl	%ebx, %ebx
	.p2align 4,,10
	.p2align 3
.L22:
	movq	%rbx, %rbp
	addq	$1, %rbx
	call	get_ncpus
	cmpq	%rax, %rbp
	jb	.L22
	call	halt
	.p2align 4,,5
	call	mp_bootothers
#APP
# 338 "main.c" 1
	sti

# 0 "" 2
#NO_APP
.L23:
	.p2align 4,,5
	jmp	.L23
	.p2align 4,,10
	.p2align 3
.L15:
	movq	16(%rbp), %rax
	movl	$3, %r13d
	movq	%rax, 40(%r14)
	jmp	.L16
	.p2align 4,,10
	.p2align 3
.L28:
	movl	$.LC5, %edi
	call	panic
	movq	8(%r14), %rdx
	jmp	.L20
	.cfi_endproc
.LFE2:
	.size	kmain, .-kmain
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

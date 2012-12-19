	.file	"mp.c"
	.text
	.p2align 4,,15
	.globl	sum
	.type	sum, @function
sum:
.LFB0:
	.cfi_startproc
	subq	$40, %rsp
	.cfi_def_cfa_offset 48
	xorl	%eax, %eax
	testl	%esi, %esi
	je	.L2
	movq	%rdi, %r9
	movq	%rdi, %rcx
	negq	%r9
	andl	$15, %r9d
	cmpl	%esi, %r9d
	cmova	%esi, %r9d
	xorl	%edx, %edx
	testl	%r9d, %r9d
	movl	%r9d, %r8d
	je	.L3
	.p2align 4,,10
	.p2align 3
.L4:
	addl	$1, %edx
	addb	(%rcx), %al
	addq	$1, %rcx
	cmpl	%edx, %r9d
	ja	.L4
	cmpl	%r9d, %esi
	je	.L2
.L3:
	movl	%esi, %r11d
	subl	%r9d, %r11d
	movl	%r11d, %r9d
	shrl	$4, %r9d
	movl	%r9d, %r10d
	sall	$4, %r10d
	testl	%r10d, %r10d
	je	.L12
	pxor	%xmm0, %xmm0
	addq	%rdi, %r8
	xorl	%ecx, %ecx
	.p2align 4,,10
	.p2align 3
.L7:
	addl	$1, %ecx
	paddb	(%r8), %xmm0
	addq	$16, %r8
	cmpl	%r9d, %ecx
	jb	.L7
	movdqa	%xmm0, %xmm1
	addl	%r10d, %edx
	psrldq	$8, %xmm1
	paddb	%xmm1, %xmm0
	movdqa	%xmm0, %xmm1
	psrldq	$4, %xmm1
	paddb	%xmm1, %xmm0
	movdqa	%xmm0, %xmm1
	psrldq	$2, %xmm1
	paddb	%xmm1, %xmm0
	movdqa	%xmm0, %xmm1
	psrldq	$1, %xmm1
	paddb	%xmm0, %xmm1
	movdqa	%xmm1, (%rsp)
	movzbl	(%rsp), %ecx
	addl	%ecx, %eax
	cmpl	%r10d, %r11d
	je	.L2
	.p2align 4,,10
	.p2align 3
.L12:
	movl	%edx, %ecx
	addl	$1, %edx
	addb	(%rdi,%rcx), %al
	cmpl	%edx, %esi
	ja	.L12
.L2:
	addq	$40, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE0:
	.size	sum, .-sum
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"_MP_"
	.text
	.p2align 4,,15
	.globl	mp_search
	.type	mp_search, @function
mp_search:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movslq	%esi, %rbp
	addq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	xorl	%ebx, %ebx
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	cmpq	%rbp, %rdi
	jae	.L18
	movq	%rdi, %rbx
	.p2align 4,,10
	.p2align 3
.L20:
	movl	$4, %edx
	movl	$.LC0, %esi
	movq	%rbx, %rdi
	call	memcmp
	testl	%eax, %eax
	jne	.L19
	movzbl	1(%rbx), %eax
	addb	(%rbx), %al
	addb	2(%rbx), %al
	addb	3(%rbx), %al
	addb	4(%rbx), %al
	addb	5(%rbx), %al
	addb	6(%rbx), %al
	addb	7(%rbx), %al
	addb	8(%rbx), %al
	addb	9(%rbx), %al
	addb	10(%rbx), %al
	addb	11(%rbx), %al
	addb	12(%rbx), %al
	addb	13(%rbx), %al
	addb	14(%rbx), %al
	addb	15(%rbx), %al
	testb	%al, %al
	je	.L18
.L19:
	addq	$16, %rbx
	cmpq	%rbp, %rbx
	jb	.L20
	xorl	%ebx, %ebx
.L18:
	addq	$8, %rsp
	.cfi_def_cfa_offset 24
	movq	%rbx, %rax
	popq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_restore 3
	popq	%rbp
	.cfi_def_cfa_offset 8
	.cfi_restore 6
	ret
	.cfi_endproc
.LFE1:
	.size	mp_search, .-mp_search
	.p2align 4,,15
	.globl	mp_bootothers
	.type	mp_bootothers, @function
mp_bootothers:
.LFB2:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	call	cli
	movzbl	cpus+1120(%rip), %edi
	call	lapic_startaps
	.p2align 4,,10
	.p2align 3
.L24:
	movzbl	cpus+1122(%rip), %eax
	testb	%al, %al
	je	.L24
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE2:
	.size	mp_bootothers, .-mp_bootothers
	.section	.rodata.str1.1
.LC1:
	.string	"APIC is NOT supported!"
.LC2:
	.string	"No MP floating pointer found"
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC3:
	.string	"Legacy MP configurations not supported"
	.section	.rodata.str1.1
.LC4:
	.string	"PIC Mode not supported\n"
.LC5:
	.string	"PCMP"
	.section	.rodata.str1.8
	.align 8
.LC6:
	.string	"Bad Configuration Table %d %d %d\n"
	.section	.rodata.str1.1
.LC7:
	.string	"IOAPIC address is not valid"
.LC8:
	.string	"Unknown config type"
.LC9:
	.string	"Too little number of CPUs"
.LC10:
	.string	"Number of CPUs: %d\n"
	.text
	.p2align 4,,15
	.globl	mp_init
	.type	mp_init, @function
mp_init:
.LFB3:
	.cfi_startproc
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	movl	$1, %edi
	pushq	%r13
	.cfi_def_cfa_offset 24
	.cfi_offset 13, -24
	pushq	%r12
	.cfi_def_cfa_offset 32
	.cfi_offset 12, -32
	pushq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	subq	$48, %rsp
	.cfi_def_cfa_offset 96
	leaq	40(%rsp), %r8
	leaq	44(%rsp), %rcx
	leaq	36(%rsp), %rdx
	leaq	32(%rsp), %rsi
	call	cpuid
	testb	$2, 41(%rsp)
	je	.L27
	movl	$27, %edi
	call	rdmsr
	testb	$8, %ah
	je	.L27
.L28:
	movl	$983040, %ebx
	.p2align 4,,10
	.p2align 3
.L31:
	movl	$4, %edx
	movl	$.LC0, %esi
	movq	%rbx, %rdi
	call	memcmp
	testl	%eax, %eax
	jne	.L29
	movzbl	1(%rbx), %eax
	addb	(%rbx), %al
	addb	2(%rbx), %al
	addb	3(%rbx), %al
	addb	4(%rbx), %al
	addb	5(%rbx), %al
	addb	6(%rbx), %al
	addb	7(%rbx), %al
	addb	8(%rbx), %al
	addb	9(%rbx), %al
	addb	10(%rbx), %al
	addb	11(%rbx), %al
	addb	12(%rbx), %al
	addb	13(%rbx), %al
	addb	14(%rbx), %al
	addb	15(%rbx), %al
	testb	%al, %al
	je	.L30
.L29:
	addq	$16, %rbx
	cmpq	$1048576, %rbx
	jne	.L31
.L32:
	movl	$.LC2, %edi
	xorl	%ebx, %ebx
	call	panic
	jmp	.L33
	.p2align 4,,10
	.p2align 3
.L27:
	movl	$.LC1, %edi
	call	panic
	.p2align 4,,3
	jmp	.L28
	.p2align 4,,10
	.p2align 3
.L30:
	testq	%rbx, %rbx
	je	.L32
.L33:
	movl	4(%rbx), %eax
	testl	%eax, %eax
	.p2align 4,,3
	je	.L34
	cmpb	$0, 11(%rbx)
	je	.L35
.L34:
	movl	$.LC3, %edi
	call	panic
.L35:
	cmpb	$0, 12(%rbx)
	js	.L84
.L36:
	movl	4(%rbx), %r12d
	movl	$4, %edx
	movl	$.LC5, %esi
	movq	%r12, %rdi
	movq	%r12, %rbx
	movq	%r12, %r14
	call	memcmp
	testl	%eax, %eax
	je	.L37
	movzwl	4(%r12), %edi
	movzbl	6(%r12), %eax
.L38:
	xorl	%r13d, %r13d
	cmpb	$4, %al
	setne	%r13b
	cmpb	$1, %al
	setne	%al
	xorl	%ebp, %ebp
	movzbl	%al, %eax
	andl	%eax, %r13d
	testl	%edi, %edi
	je	.L49
.L47:
	movq	%r12, %rcx
	negq	%rcx
	andl	$15, %ecx
	cmpl	%ecx, %edi
	cmovbe	%edi, %ecx
	xorl	%edx, %edx
	xorl	%eax, %eax
	testl	%ecx, %ecx
	movl	%ecx, %esi
	je	.L50
	.p2align 4,,10
	.p2align 3
.L51:
	addl	$1, %eax
	addb	(%r12), %dl
	addq	$1, %r12
	cmpl	%eax, %ecx
	ja	.L51
	cmpl	%ecx, %edi
	je	.L52
.L50:
	movl	%edi, %r10d
	subl	%ecx, %r10d
	movl	%r10d, %r8d
	shrl	$4, %r8d
	movl	%r8d, %r9d
	sall	$4, %r9d
	testl	%r9d, %r9d
	je	.L74
	pxor	%xmm0, %xmm0
	addq	%rbx, %rsi
	xorl	%ecx, %ecx
	.p2align 4,,10
	.p2align 3
.L54:
	addl	$1, %ecx
	paddb	(%rsi), %xmm0
	addq	$16, %rsi
	cmpl	%ecx, %r8d
	ja	.L54
	movdqa	%xmm0, %xmm1
	addl	%r9d, %eax
	psrldq	$8, %xmm1
	paddb	%xmm1, %xmm0
	movdqa	%xmm0, %xmm1
	psrldq	$4, %xmm1
	paddb	%xmm1, %xmm0
	movdqa	%xmm0, %xmm1
	psrldq	$2, %xmm1
	paddb	%xmm1, %xmm0
	movdqa	%xmm0, %xmm1
	psrldq	$1, %xmm1
	paddb	%xmm0, %xmm1
	movdqa	%xmm1, (%rsp)
	movzbl	(%rsp), %ecx
	addl	%ecx, %edx
	cmpl	%r9d, %r10d
	je	.L52
	.p2align 4,,10
	.p2align 3
.L74:
	movl	%eax, %ecx
	addl	$1, %eax
	addb	(%rbx,%rcx), %dl
	cmpl	%eax, %edi
	ja	.L74
.L52:
	xorl	%ebp, %ebp
	testb	%dl, %dl
	setne	%bpl
.L49:
	movq	%r14, %rdi
	movl	$4, %edx
	movl	$.LC5, %esi
	call	memcmp
	xorl	%edx, %edx
	testl	%eax, %eax
	movl	%r13d, %r8d
	setne	%dl
	movl	%ebp, %ecx
	movl	$4, %esi
	movl	$.LC6, %edi
	xorl	%eax, %eax
	call	cprintk
#APP
# 146 "mp.c" 1
	cli;hlt
	
# 0 "" 2
#NO_APP
	movzwl	4(%rbx), %r9d
.L48:
	leaq	44(%rbx), %r12
	xorl	%ebp, %ebp
	movzwl	%r9w, %edx
	.p2align 4,,10
	.p2align 3
.L83:
	leaq	(%rbx,%rdx), %rax
	cmpq	%rax, %r12
	jae	.L85
.L67:
	cmpb	$4, (%r12)
	jbe	.L86
	movl	$.LC8, %edi
	call	panic
	movzwl	4(%rbx), %edx
	leaq	(%rbx,%rdx), %rax
	cmpq	%rax, %r12
	jb	.L67
	.p2align 4,,10
	.p2align 3
.L85:
	movq	ncpus(%rip), %rdx
	cmpq	$3, %rdx
	jbe	.L87
.L68:
	xorl	%eax, %eax
	movl	$14, %esi
	movl	$.LC10, %edi
	call	cprintk
	testq	%rbp, %rbp
	movl	$4273995776, %eax
	je	.L69
	movl	4(%rbp), %eax
.L69:
	movq	%rax, ioapicva(%rip)
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	popq	%rbx
	.cfi_def_cfa_offset 40
	.cfi_restore 3
	popq	%rbp
	.cfi_def_cfa_offset 32
	.cfi_restore 6
	popq	%r12
	.cfi_def_cfa_offset 24
	.cfi_restore 12
	popq	%r13
	.cfi_def_cfa_offset 16
	.cfi_restore 13
	popq	%r14
	.cfi_def_cfa_offset 8
	.cfi_restore 14
	ret
	.p2align 4,,10
	.p2align 3
.L86:
	.cfi_restore_state
	movzbl	(%r12), %eax
	jmp	*.L64(,%rax,8)
	.section	.rodata
	.align 8
	.align 4
.L64:
	.quad	.L59
	.quad	.L60
	.quad	.L61
	.quad	.L60
	.quad	.L60
	.text
	.p2align 4,,10
	.p2align 3
.L60:
	addq	$8, %r12
	jmp	.L83
	.p2align 4,,10
	.p2align 3
.L59:
	movq	ncpus(%rip), %rax
	imulq	$1120, %rax, %rdx
	movb	%al, cpus+1(%rdx)
	movzbl	1(%r12), %ecx
	addq	$1, %rax
	movq	%rax, ncpus(%rip)
	addq	$20, %r12
	movb	%cl, cpus(%rdx)
	movzwl	4(%rbx), %edx
	jmp	.L83
	.p2align 4,,10
	.p2align 3
.L61:
	cmpl	$-20971520, 4(%r12)
	je	.L66
	movl	$.LC7, %edi
	call	panic
	movzwl	4(%rbx), %edx
.L66:
	movq	%r12, %rbp
	addq	$8, %r12
	jmp	.L83
	.p2align 4,,10
	.p2align 3
.L37:
	movzwl	4(%r12), %r9d
	movzwl	%r9w, %edi
	testl	%edi, %edi
	je	.L39
	movq	%r12, %r8
	negq	%r8
	andl	$15, %r8d
	cmpl	%r8d, %edi
	cmovbe	%edi, %r8d
	testl	%r8d, %r8d
	movl	%r8d, %esi
	je	.L70
	movq	%r12, %rcx
	xorl	%edx, %edx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L41:
	addl	$1, %eax
	addb	(%rcx), %dl
	addq	$1, %rcx
	cmpl	%eax, %r8d
	ja	.L41
	cmpl	%r8d, %edi
	je	.L42
.L40:
	movl	%edi, %r11d
	subl	%r8d, %r11d
	movl	%r11d, %r8d
	shrl	$4, %r8d
	movl	%r8d, %r10d
	sall	$4, %r10d
	testl	%r10d, %r10d
	je	.L75
	pxor	%xmm0, %xmm0
	addq	%r12, %rsi
	xorl	%ecx, %ecx
	.p2align 4,,10
	.p2align 3
.L44:
	addl	$1, %ecx
	paddb	(%rsi), %xmm0
	addq	$16, %rsi
	cmpl	%r8d, %ecx
	jb	.L44
	movdqa	%xmm0, %xmm1
	addl	%r10d, %eax
	psrldq	$8, %xmm1
	paddb	%xmm1, %xmm0
	movdqa	%xmm0, %xmm1
	psrldq	$4, %xmm1
	paddb	%xmm1, %xmm0
	movdqa	%xmm0, %xmm1
	psrldq	$2, %xmm1
	paddb	%xmm1, %xmm0
	movdqa	%xmm0, %xmm1
	psrldq	$1, %xmm1
	paddb	%xmm0, %xmm1
	movdqa	%xmm1, 16(%rsp)
	movzbl	16(%rsp), %ecx
	addl	%ecx, %edx
	cmpl	%r10d, %r11d
	je	.L42
	.p2align 4,,10
	.p2align 3
.L75:
	movl	%eax, %ecx
	addl	$1, %eax
	addb	(%r12,%rcx), %dl
	cmpl	%eax, %edi
	ja	.L75
.L42:
	testb	%dl, %dl
	je	.L39
	movzbl	6(%r12), %eax
	xorl	%r13d, %r13d
	cmpb	$4, %al
	setne	%r13b
	cmpb	$1, %al
	setne	%al
	movzbl	%al, %eax
	andl	%eax, %r13d
	jmp	.L47
	.p2align 4,,10
	.p2align 3
.L87:
	movl	$.LC9, %edi
	call	panic
	movq	ncpus(%rip), %rdx
	jmp	.L68
	.p2align 4,,10
	.p2align 3
.L39:
	movzbl	6(%r12), %eax
	cmpb	$4, %al
	je	.L48
	cmpb	$1, %al
	je	.L48
	jmp	.L38
.L84:
	movl	$.LC4, %edi
	call	panic
	.p2align 4,,3
	jmp	.L36
.L70:
	xorl	%edx, %edx
	xorl	%eax, %eax
	jmp	.L40
	.cfi_endproc
.LFE3:
	.size	mp_init, .-mp_init
	.globl	ncpus
	.bss
	.align 16
	.type	ncpus, @object
	.size	ncpus, 8
ncpus:
	.zero	8
	.comm	cpus,1146880,32
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

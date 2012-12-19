	.file	"lapic.c"
	.text
	.p2align 4,,15
	.globl	lapic_read
	.type	lapic_read, @function
lapic_read:
.LFB0:
	.cfi_startproc
	movl	$4276092928, %eax
	movl	%edi, %edi
	addq	%rax, %rdi
	movl	(%rdi), %eax
	ret
	.cfi_endproc
.LFE0:
	.size	lapic_read, .-lapic_read
	.p2align 4,,15
	.globl	lapic_eoi
	.type	lapic_eoi, @function
lapic_eoi:
.LFB1:
	.cfi_startproc
	xorl	%eax, %eax
	movabsl	%eax, 4276093104
	movabsl	4276092960, %eax
	ret
	.cfi_endproc
.LFE1:
	.size	lapic_eoi, .-lapic_eoi
	.p2align 4,,15
	.globl	lapic_write
	.type	lapic_write, @function
lapic_write:
.LFB2:
	.cfi_startproc
	movl	$4276092928, %eax
	movl	%edi, %edi
	addq	%rax, %rdi
	movl	%esi, (%rdi)
	movabsl	4276092960, %eax
	ret
	.cfi_endproc
.LFE2:
	.size	lapic_write, .-lapic_write
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC0:
	.string	"Failed to get cpu information. boot/stage2/lapic.c"
	.align 8
.LC1:
	.string	"Why do you try to boot a booted processor?!"
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC2:
	.string	"apic_icr_wait: wedged?\n"
.LC3:
	.string	"unable to send init error r"
	.text
	.p2align 4,,15
	.globl	lapic_startaps
	.type	lapic_startaps, @function
lapic_startaps:
.LFB5:
	.cfi_startproc
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	movq	%rdi, %rbx
	call	get_cpu_info
	testq	%rax, %rax
	movq	%rax, %rbp
	je	.L23
.L5:
	movzbl	2(%rbp), %eax
	testb	%al, %al
	jne	.L24
.L6:
	movl	$112, %esi
	movl	$15, %edi
	movl	$100000, %r12d
	call	outb
	movl	$113, %esi
	movl	$10, %edi
	call	outb
	movq	%rbp, 651776
	movl	%ebx, %ebp
	movw	$0, 1127
	sall	$24, %ebp
	movw	$-24832, 1129
	movl	%ebp, %eax
	movabsl	%eax, 4276093712
	movabsl	4276092960, %eax
	movl	%ebx, %eax
	orb	$197, %ah
	movabsl	%eax, 4276093696
	movabsl	4276092960, %eax
	jmp	.L7
	.p2align 4,,10
	.p2align 3
.L9:
	call	nop_pause
	subl	$1, %r12d
	je	.L25
.L7:
	movabsl	4276093696, %eax
	testb	$16, %ah
	jne	.L9
.L8:
	movl	$10, %edi
	call	timer_delay
	movl	%ebx, %eax
	movl	$100000, %ebx
	orb	$133, %ah
	movabsl	%eax, 4276093696
	movabsl	4276092960, %eax
	jmp	.L10
	.p2align 4,,10
	.p2align 3
.L12:
	call	nop_pause
	subl	$1, %ebx
	je	.L26
.L10:
	movabsl	4276093696, %eax
	testb	$16, %ah
	jne	.L12
.L11:
	movabsl	4276093696, %eax
	testb	$16, %ah
	je	.L13
	movl	$.LC3, %edi
	call	panic
.L13:
	movl	$10, %edi
	movl	$2, %ebx
	call	timer_delay
.L14:
	movl	$100000, %r12d
	jmp	.L15
	.p2align 4,,10
	.p2align 3
.L17:
	call	nop_pause
	subl	$1, %r12d
	je	.L27
.L15:
	movabsl	4276093696, %eax
	testb	$16, %ah
	jne	.L17
.L16:
	movl	%ebp, %eax
	movl	$1, %edi
	movabsl	%eax, 4276093712
	movabsl	4276092960, %eax
	movl	$1695, %eax
	movabsl	%eax, 4276093696
	movabsl	4276092960, %eax
	call	timer_delay
	subl	$1, %ebx
	jne	.L14
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	.cfi_restore 3
	popq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_restore 6
	popq	%r12
	.cfi_def_cfa_offset 8
	.cfi_restore 12
	ret
.L27:
	.cfi_restore_state
	movl	$4, %esi
	movl	$.LC2, %edi
	xorl	%eax, %eax
	call	cprintk
	jmp	.L16
.L24:
	movl	$.LC1, %edi
	call	panic
	.p2align 4,,3
	jmp	.L6
.L26:
	movl	$4, %esi
	movl	$.LC2, %edi
	xorl	%eax, %eax
	call	cprintk
	jmp	.L11
.L25:
	movl	$4, %esi
	movl	$.LC2, %edi
	xorl	%eax, %eax
	call	cprintk
	jmp	.L8
.L23:
	movl	$.LC0, %edi
	call	panic
	.p2align 4,,3
	jmp	.L5
	.cfi_endproc
.LFE5:
	.size	lapic_startaps, .-lapic_startaps
	.p2align 4,,15
	.globl	lapic_init
	.type	lapic_init, @function
lapic_init:
.LFB6:
	.cfi_startproc
	movl	$11, %eax
	movl	$4276092960, %ecx
	movabsl	%eax, 4276093920
	movl	$268435455, %eax
	movl	(%rcx), %edx
	movabsl	%eax, 4276093824
	movl	$131104, %eax
	movl	(%rcx), %edx
	movabsl	%eax, 4276093728
	movl	(%rcx), %eax
	ret
	.cfi_endproc
.LFE6:
	.size	lapic_init, .-lapic_init
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

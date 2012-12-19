	.file	"main.c"
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"Welcome to VMM\n"
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC1:
	.string	"       v       v  u    u         oo      sss\n"
	.align 8
.LC2:
	.string	"        v     v   u    u        o  o    s\n"
	.align 8
.LC3:
	.string	"         v   v    u    u  ===   o  o     s\n"
	.align 8
.LC4:
	.string	"          v v     u    u        o  o      s\n"
	.align 8
.LC5:
	.string	"           v       uuuu          oo     sss\n"
	.section	.rodata.str1.1
.LC6:
	.string	"\n"
	.text
	.p2align 4,,15
	.globl	print_logo
	.type	print_logo, @function
print_logo:
.LFB0:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	$14, %esi
	movl	$.LC0, %edi
	xorl	%eax, %eax
	call	cprintk
	movl	$14, %esi
	movl	$.LC1, %edi
	xorl	%eax, %eax
	call	cprintk
	movl	$14, %esi
	movl	$.LC2, %edi
	xorl	%eax, %eax
	call	cprintk
	movl	$14, %esi
	movl	$.LC3, %edi
	xorl	%eax, %eax
	call	cprintk
	movl	$14, %esi
	movl	$.LC4, %edi
	xorl	%eax, %eax
	call	cprintk
	movl	$14, %esi
	movl	$.LC5, %edi
	xorl	%eax, %eax
	call	cprintk
	movl	$14, %esi
	movl	$.LC6, %edi
	xorl	%eax, %eax
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	jmp	cprintk
	.cfi_endproc
.LFE0:
	.size	print_logo, .-print_logo
	.section	.rodata.str1.1
.LC7:
	.string	"My cpuid is = %d\n"
	.text
	.p2align 4,,15
	.globl	vmm_main
	.type	vmm_main, @function
vmm_main:
.LFB1:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	xorl	%eax, %eax
	movq	%rdi, %rbx
	call	con_init
	movzbl	1(%rbx), %edx
	movl	$10, %esi
	xorl	%eax, %eax
	movl	$.LC7, %edi
	call	cprintk
	movzbl	1(%rbx), %edi
	call	create_new_gdt
	call	interrupt_init
	call	print_logo
	movb	$1, 2(%rbx)
	.p2align 4,,10
	.p2align 3
.L3:
	call	halt
	jmp	.L3
	.cfi_endproc
.LFE1:
	.size	vmm_main, .-vmm_main
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

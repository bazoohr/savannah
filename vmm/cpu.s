	.file	"cpu.c"
	.text
	.p2align 4,,15
	.globl	rdmsr
	.type	rdmsr, @function
rdmsr:
.LFB0:
	.cfi_startproc
	movl	%edi, %ecx
#APP
# 8 "cpu.c" 1
	rdmsr; mfence
# 0 "" 2
#NO_APP
	movq	%rax, %rdi
	movq	%rdx, %rax
	salq	$32, %rax
	orq	%rdi, %rax
	ret
	.cfi_endproc
.LFE0:
	.size	rdmsr, .-rdmsr
	.p2align 4,,15
	.globl	wrmsr
	.type	wrmsr, @function
wrmsr:
.LFB1:
	.cfi_startproc
	movq	%rsi, %rdx
	movl	%esi, %eax
	movl	%edi, %ecx
	shrq	$32, %rdx
#APP
# 15 "cpu.c" 1
	wrmsr
# 0 "" 2
#NO_APP
	ret
	.cfi_endproc
.LFE1:
	.size	wrmsr, .-wrmsr
	.p2align 4,,15
	.globl	lcr0
	.type	lcr0, @function
lcr0:
.LFB2:
	.cfi_startproc
#APP
# 22 "cpu.c" 1
	movq %rdi,%cr0
# 0 "" 2
#NO_APP
	ret
	.cfi_endproc
.LFE2:
	.size	lcr0, .-lcr0
	.p2align 4,,15
	.globl	rcr0
	.type	rcr0, @function
rcr0:
.LFB3:
	.cfi_startproc
#APP
# 29 "cpu.c" 1
	movq %cr0,%rax
# 0 "" 2
#NO_APP
	ret
	.cfi_endproc
.LFE3:
	.size	rcr0, .-rcr0
	.p2align 4,,15
	.globl	rcr2
	.type	rcr2, @function
rcr2:
.LFB4:
	.cfi_startproc
#APP
# 37 "cpu.c" 1
	movq %cr2,%rax
# 0 "" 2
#NO_APP
	ret
	.cfi_endproc
.LFE4:
	.size	rcr2, .-rcr2
	.p2align 4,,15
	.globl	lcr3
	.type	lcr3, @function
lcr3:
.LFB5:
	.cfi_startproc
#APP
# 44 "cpu.c" 1
	movq %rdi,%cr3
# 0 "" 2
#NO_APP
	ret
	.cfi_endproc
.LFE5:
	.size	lcr3, .-lcr3
	.p2align 4,,15
	.globl	rcr3
	.type	rcr3, @function
rcr3:
.LFB6:
	.cfi_startproc
#APP
# 51 "cpu.c" 1
	movq %cr3,%rax
# 0 "" 2
#NO_APP
	ret
	.cfi_endproc
.LFE6:
	.size	rcr3, .-rcr3
	.p2align 4,,15
	.globl	lcr4
	.type	lcr4, @function
lcr4:
.LFB7:
	.cfi_startproc
#APP
# 58 "cpu.c" 1
	movq %rdi,%cr4
# 0 "" 2
#NO_APP
	ret
	.cfi_endproc
.LFE7:
	.size	lcr4, .-lcr4
	.p2align 4,,15
	.globl	rcr4
	.type	rcr4, @function
rcr4:
.LFB8:
	.cfi_startproc
#APP
# 65 "cpu.c" 1
	movq %cr4,%rax
# 0 "" 2
#NO_APP
	ret
	.cfi_endproc
.LFE8:
	.size	rcr4, .-rcr4
	.p2align 4,,15
	.globl	nop_pause
	.type	nop_pause, @function
nop_pause:
.LFB9:
	.cfi_startproc
#APP
# 72 "cpu.c" 1
	pause
# 0 "" 2
#NO_APP
	ret
	.cfi_endproc
.LFE9:
	.size	nop_pause, .-nop_pause
	.p2align 4,,15
	.globl	cache_flush
	.type	cache_flush, @function
cache_flush:
.LFB10:
	.cfi_startproc
#APP
# 78 "cpu.c" 1
	wbinvd
# 0 "" 2
#NO_APP
	ret
	.cfi_endproc
.LFE10:
	.size	cache_flush, .-cache_flush
	.p2align 4,,15
	.globl	tlb_flush_global
	.type	tlb_flush_global, @function
tlb_flush_global:
.LFB11:
	.cfi_startproc
#APP
# 65 "cpu.c" 1
	movq %cr4,%rax
# 0 "" 2
#NO_APP
	testb	$-128, %al
	jne	.L16
#APP
# 51 "cpu.c" 1
	movq %cr3,%rax
# 0 "" 2
# 44 "cpu.c" 1
	movq %rax,%cr3
# 0 "" 2
#NO_APP
	ret
	.p2align 4,,10
	.p2align 3
.L16:
	movq	%rax, %rdx
	andb	$127, %dl
#APP
# 58 "cpu.c" 1
	movq %rdx,%cr4
# 0 "" 2
# 58 "cpu.c" 1
	movq %rax,%cr4
# 0 "" 2
#NO_APP
	ret
	.cfi_endproc
.LFE11:
	.size	tlb_flush_global, .-tlb_flush_global
	.p2align 4,,15
	.globl	cpuid
	.type	cpuid, @function
cpuid:
.LFB12:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rdx, %r10
	movq	%rcx, %r9
	movl	%edi, %eax
#APP
# 100 "cpu.c" 1
	cpuid
# 0 "" 2
#NO_APP
	testq	%rsi, %rsi
	je	.L18
	movl	%eax, (%rsi)
.L18:
	testq	%r10, %r10
	je	.L19
	movl	%ebx, (%r10)
.L19:
	testq	%r9, %r9
	je	.L20
	movl	%ecx, (%r9)
.L20:
	testq	%r8, %r8
	je	.L17
	movl	%edx, (%r8)
.L17:
	popq	%rbx
	.cfi_def_cfa_offset 8
	.cfi_restore 3
	ret
	.cfi_endproc
.LFE12:
	.size	cpuid, .-cpuid
	.p2align 4,,15
	.globl	halt
	.type	halt, @function
halt:
.LFB13:
	.cfi_startproc
	.p2align 4,,10
	.p2align 3
.L23:
#APP
# 117 "cpu.c" 1
	cli;hlt

# 0 "" 2
#NO_APP
	jmp	.L23
	.cfi_endproc
.LFE13:
	.size	halt, .-halt
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

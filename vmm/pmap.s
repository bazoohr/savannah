	.file	"pmap.c"
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC0:
	.string	"IO mapped memory region can not be beyond 4GB"
	.text
	.p2align 4,,15
	.globl	map_iomem
	.type	map_iomem, @function
map_iomem:
.LFB0:
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
	movq	kernel_pml4(%rip), %rbx
	movq	(%rbx), %rax
	movq	%rax, %r12
	andq	$-4096, %r12
	testq	%rax, %rax
	movq	24(%r12), %rbp
	movq	%rbp, %rdx
	je	.L43
	andq	$-4096, %rbp
	testq	%rdx, %rdx
	je	.L44
.L4:
	movl	$4273995923, %r12d
	movq	%r12, 4016(%rbp)
	movq	(%rbx), %rax
	movq	%rax, %r12
	andq	$-4096, %r12
	testq	%rax, %rax
	movq	24(%r12), %rbp
	movq	%rbp, %rdx
	je	.L45
.L5:
	andq	$-4096, %rbp
	testq	%rdx, %rdx
	je	.L46
.L7:
	movl	$4276093075, %r11d
	movq	%r11, 4024(%rbp)
	movq	(%rbx), %rax
	movq	%rax, %r12
	andq	$-4096, %r12
	testq	%rax, %rax
	movq	24(%r12), %rbp
	movq	%rbp, %rdx
	je	.L47
.L8:
	andq	$-4096, %rbp
	testq	%rdx, %rdx
	je	.L48
.L10:
	movl	$4278190227, %r10d
	movq	%r10, 4032(%rbp)
	movq	(%rbx), %rax
	movq	%rax, %r12
	andq	$-4096, %r12
	testq	%rax, %rax
	movq	24(%r12), %rbp
	movq	%rbp, %rdx
	je	.L49
.L11:
	andq	$-4096, %rbp
	testq	%rdx, %rdx
	je	.L50
.L13:
	movl	$4280287379, %r9d
	movq	%r9, 4040(%rbp)
	movq	(%rbx), %rax
	movq	%rax, %r12
	andq	$-4096, %r12
	testq	%rax, %rax
	movq	24(%r12), %rbp
	movq	%rbp, %rdx
	je	.L51
.L14:
	andq	$-4096, %rbp
	testq	%rdx, %rdx
	je	.L52
.L16:
	movl	$4282384531, %r8d
	movq	%r8, 4048(%rbp)
	movq	(%rbx), %rax
	movq	%rax, %r12
	andq	$-4096, %r12
	testq	%rax, %rax
	movq	24(%r12), %rbp
	movq	%rbp, %rdx
	je	.L53
.L17:
	andq	$-4096, %rbp
	testq	%rdx, %rdx
	je	.L54
.L19:
	movl	$4284481683, %edi
	movq	%rdi, 4056(%rbp)
	movq	(%rbx), %rax
	movq	%rax, %r12
	andq	$-4096, %r12
	testq	%rax, %rax
	movq	24(%r12), %rbp
	movq	%rbp, %rdx
	je	.L55
.L20:
	andq	$-4096, %rbp
	testq	%rdx, %rdx
	je	.L56
.L22:
	movl	$4286578835, %esi
	movq	%rsi, 4064(%rbp)
	movq	(%rbx), %rax
	movq	%rax, %r12
	andq	$-4096, %r12
	testq	%rax, %rax
	movq	24(%r12), %rbp
	movq	%rbp, %rdx
	je	.L57
.L23:
	andq	$-4096, %rbp
	testq	%rdx, %rdx
	je	.L58
.L25:
	movl	$4288675987, %ecx
	movq	%rcx, 4072(%rbp)
	movq	(%rbx), %rax
	movq	%rax, %r12
	andq	$-4096, %r12
	testq	%rax, %rax
	movq	24(%r12), %rbp
	movq	%rbp, %rdx
	je	.L59
.L26:
	andq	$-4096, %rbp
	testq	%rdx, %rdx
	je	.L60
.L28:
	movl	$4290773139, %edx
	movq	%rdx, 4080(%rbp)
	movq	(%rbx), %rax
	movq	%rax, %rbp
	andq	$-4096, %rbp
	testq	%rax, %rax
	movq	24(%rbp), %rbx
	movq	%rbx, %rdx
	je	.L61
.L29:
	andq	$-4096, %rbx
	testq	%rdx, %rdx
	je	.L62
.L31:
	movl	$4292870291, %eax
	movq	%rax, 4088(%rbx)
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
	.p2align 4,,10
	.p2align 3
.L43:
	.cfi_restore_state
	movl	$.LC0, %edi
	andq	$-4096, %rbp
	call	panic
	movq	24(%r12), %rdx
	testq	%rdx, %rdx
	jne	.L4
.L44:
	movl	$1, %edi
	call	alloc_mem_pages
	movl	$4096, %edx
	movq	%rax, %rbp
	xorl	%esi, %esi
	movq	%rax, %rdi
	call	memset
	movq	%rbp, %rax
	orq	$3, %rax
	movq	%rax, 24(%r12)
	jmp	.L4
	.p2align 4,,10
	.p2align 3
.L62:
	movl	$1, %edi
	call	alloc_mem_pages
	movl	$4096, %edx
	movq	%rax, %rbx
	xorl	%esi, %esi
	movq	%rax, %rdi
	call	memset
	movq	%rbx, %rax
	orq	$3, %rax
	movq	%rax, 24(%rbp)
	jmp	.L31
	.p2align 4,,10
	.p2align 3
.L61:
	movl	$.LC0, %edi
	call	panic
	movq	24(%rbp), %rdx
	jmp	.L29
	.p2align 4,,10
	.p2align 3
.L60:
	movl	$1, %edi
	call	alloc_mem_pages
	movl	$4096, %edx
	movq	%rax, %rbp
	xorl	%esi, %esi
	movq	%rax, %rdi
	call	memset
	movq	%rbp, %rax
	orq	$3, %rax
	movq	%rax, 24(%r12)
	jmp	.L28
	.p2align 4,,10
	.p2align 3
.L59:
	movl	$.LC0, %edi
	call	panic
	movq	24(%r12), %rdx
	jmp	.L26
	.p2align 4,,10
	.p2align 3
.L58:
	movl	$1, %edi
	call	alloc_mem_pages
	movl	$4096, %edx
	movq	%rax, %rbp
	xorl	%esi, %esi
	movq	%rax, %rdi
	call	memset
	movq	%rbp, %rax
	orq	$3, %rax
	movq	%rax, 24(%r12)
	jmp	.L25
	.p2align 4,,10
	.p2align 3
.L57:
	movl	$.LC0, %edi
	call	panic
	movq	24(%r12), %rdx
	jmp	.L23
	.p2align 4,,10
	.p2align 3
.L56:
	movl	$1, %edi
	call	alloc_mem_pages
	movl	$4096, %edx
	movq	%rax, %rbp
	xorl	%esi, %esi
	movq	%rax, %rdi
	call	memset
	movq	%rbp, %rax
	orq	$3, %rax
	movq	%rax, 24(%r12)
	jmp	.L22
	.p2align 4,,10
	.p2align 3
.L55:
	movl	$.LC0, %edi
	call	panic
	movq	24(%r12), %rdx
	jmp	.L20
	.p2align 4,,10
	.p2align 3
.L54:
	movl	$1, %edi
	call	alloc_mem_pages
	movl	$4096, %edx
	movq	%rax, %rbp
	xorl	%esi, %esi
	movq	%rax, %rdi
	call	memset
	movq	%rbp, %rax
	orq	$3, %rax
	movq	%rax, 24(%r12)
	jmp	.L19
	.p2align 4,,10
	.p2align 3
.L53:
	movl	$.LC0, %edi
	call	panic
	movq	24(%r12), %rdx
	jmp	.L17
	.p2align 4,,10
	.p2align 3
.L52:
	movl	$1, %edi
	call	alloc_mem_pages
	movl	$4096, %edx
	movq	%rax, %rbp
	xorl	%esi, %esi
	movq	%rax, %rdi
	call	memset
	movq	%rbp, %rax
	orq	$3, %rax
	movq	%rax, 24(%r12)
	jmp	.L16
	.p2align 4,,10
	.p2align 3
.L51:
	movl	$.LC0, %edi
	call	panic
	movq	24(%r12), %rdx
	jmp	.L14
	.p2align 4,,10
	.p2align 3
.L50:
	movl	$1, %edi
	call	alloc_mem_pages
	movl	$4096, %edx
	movq	%rax, %rbp
	xorl	%esi, %esi
	movq	%rax, %rdi
	call	memset
	movq	%rbp, %rax
	orq	$3, %rax
	movq	%rax, 24(%r12)
	jmp	.L13
	.p2align 4,,10
	.p2align 3
.L49:
	movl	$.LC0, %edi
	call	panic
	movq	24(%r12), %rdx
	jmp	.L11
	.p2align 4,,10
	.p2align 3
.L48:
	movl	$1, %edi
	call	alloc_mem_pages
	movl	$4096, %edx
	movq	%rax, %rbp
	xorl	%esi, %esi
	movq	%rax, %rdi
	call	memset
	movq	%rbp, %rax
	orq	$3, %rax
	movq	%rax, 24(%r12)
	jmp	.L10
	.p2align 4,,10
	.p2align 3
.L47:
	movl	$.LC0, %edi
	call	panic
	movq	24(%r12), %rdx
	jmp	.L8
	.p2align 4,,10
	.p2align 3
.L46:
	movl	$1, %edi
	call	alloc_mem_pages
	movl	$4096, %edx
	movq	%rax, %rbp
	xorl	%esi, %esi
	movq	%rax, %rdi
	call	memset
	movq	%rbp, %rax
	orq	$3, %rax
	movq	%rax, 24(%r12)
	jmp	.L7
	.p2align 4,,10
	.p2align 3
.L45:
	movl	$.LC0, %edi
	call	panic
	movq	24(%r12), %rdx
	jmp	.L5
	.cfi_endproc
.LFE0:
	.size	map_iomem, .-map_iomem
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

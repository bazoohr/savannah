	.file	"memory.c"
	.text
	.p2align 4,,15
	.globl	alloc_mem_pages
	.type	alloc_mem_pages, @function
alloc_mem_pages:
.LFB4:
	.cfi_startproc
	movq	blist(%rip), %rdx
	xorl	%eax, %eax
	testq	%rdx, %rdx
	je	.L2
	.p2align 4,,10
	.p2align 3
.L14:
	movl	20(%rdx), %eax
	testl	%eax, %eax
	je	.L3
	movl	16(%rdx), %ecx
	movslq	%ecx, %rax
	cmpq	%rdi, %rax
	jae	.L4
.L3:
	movq	24(%rdx), %rdx
	testq	%rdx, %rdx
	jne	.L14
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L4:
	cmpq	%rax, %rdi
	je	.L6
	movq	all_nodes(%rip), %r9
	xorl	%esi, %esi
	testq	%r9, %r9
	je	.L7
	movq	block_node_array(%rip), %rsi
	movl	$1, %eax
	movl	16(%rsi), %r11d
	leaq	40(%rsi), %r8
	testl	%r11d, %r11d
	jne	.L10
	jmp	.L7
	.p2align 4,,10
	.p2align 3
.L11:
	movq	%r8, %rsi
	addq	$1, %rax
	addq	$40, %r8
	movl	16(%rsi), %r10d
	testl	%r10d, %r10d
	je	.L7
.L10:
	cmpq	%r9, %rax
	jne	.L11
	xorl	%esi, %esi
.L7:
	movq	%rdi, %rax
	movq	8(%rdx), %r8
	subl	%edi, %ecx
	salq	$12, %rax
	addq	(%rdx), %rax
	movl	%ecx, 16(%rsi)
	movq	24(%rdx), %rcx
	movl	$1, 20(%rsi)
	movq	%r8, 8(%rsi)
	movq	%rdx, 32(%rsi)
	movl	%edi, 16(%rdx)
	movl	$0, 20(%rdx)
	movq	%rax, (%rsi)
	movq	%rax, 8(%rdx)
	movq	(%rdx), %rax
	movq	%rcx, 24(%rsi)
	movq	%rsi, 24(%rdx)
.L2:
	rep
	ret
.L6:
	movl	$0, 20(%rdx)
	movq	(%rdx), %rax
	ret
	.cfi_endproc
.LFE4:
	.size	alloc_mem_pages, .-alloc_mem_pages
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC0:
	.string	"=============== MEMORY LAYOUT ===============\n"
	.align 8
.LC1:
	.string	"block address = %x pages = %x free = %d\n"
	.text
	.p2align 4,,15
	.globl	get_memory_status
	.type	get_memory_status, @function
get_memory_status:
.LFB5:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	blist(%rip), %rbx
	xorl	%eax, %eax
	movl	$11, %esi
	movl	$.LC0, %edi
	call	cprintk
	testq	%rbx, %rbx
	je	.L19
	.p2align 4,,10
	.p2align 3
.L22:
	movl	16(%rbx), %ecx
	movl	20(%rbx), %r8d
	xorl	%eax, %eax
	movq	(%rbx), %rdx
	movl	$11, %esi
	movl	$.LC1, %edi
	call	cprintk
	movq	24(%rbx), %rbx
	testq	%rbx, %rbx
	jne	.L22
.L19:
	popq	%rbx
	.cfi_def_cfa_offset 8
	.cfi_restore 3
	ret
	.cfi_endproc
.LFE5:
	.size	get_memory_status, .-get_memory_status
	.p2align 4,,15
	.globl	free_mem_pages
	.type	free_mem_pages, @function
free_mem_pages:
.LFB6:
	.cfi_startproc
	movq	blist(%rip), %rax
	testq	%rax, %rax
	je	.L24
	movq	all_nodes(%rip), %r11
	movq	block_node_array(%rip), %r10
	leaq	1(%r11), %r9
	jmp	.L39
	.p2align 4,,10
	.p2align 3
.L45:
	movq	24(%rax), %rax
	testq	%rax, %rax
	je	.L24
.L39:
	cmpq	%rdi, (%rax)
	jne	.L45
	movl	20(%rax), %edx
	testl	%edx, %edx
	jne	.L24
	movq	32(%rax), %rsi
	testq	%rsi, %rsi
	je	.L44
	cmpl	$1, 20(%rsi)
	je	.L30
.L44:
	movq	24(%rax), %rdx
	testq	%rdx, %rdx
	je	.L34
.L35:
	cmpl	$1, 20(%rdx)
	je	.L46
.L34:
	movl	$1, 20(%rax)
	movq	%rdx, %rax
	testq	%rax, %rax
	jne	.L39
.L24:
	rep
	ret
	.p2align 4,,10
	.p2align 3
.L46:
	movl	16(%rdx), %ecx
	addl	%ecx, 16(%rax)
	movl	$1, %esi
	movq	8(%rdx), %rcx
	movq	%rcx, 8(%rax)
	xorl	%ecx, %ecx
	testq	%r11, %r11
	jne	.L38
	jmp	.L36
	.p2align 4,,10
	.p2align 3
.L37:
	leaq	1(%rsi), %r8
	cmpq	%r9, %r8
	je	.L36
.L42:
	movq	%rsi, %rcx
	movq	%r8, %rsi
.L38:
	leaq	(%rcx,%rcx,4), %rcx
	leaq	(%r10,%rcx,8), %rcx
	cmpq	%rcx, %rdx
	jne	.L37
	leaq	1(%rsi), %r8
	movl	$0, 16(%rdx)
	cmpq	%r9, %r8
	jne	.L42
	.p2align 4,,10
	.p2align 3
.L36:
	movq	24(%rdx), %rdx
	testq	%rdx, %rdx
	movq	%rdx, 24(%rax)
	je	.L34
	movq	%rax, 32(%rdx)
	jmp	.L34
	.p2align 4,,10
	.p2align 3
.L30:
	movq	24(%rsi), %r8
	movl	$1, %edx
	movl	16(%r8), %eax
	addl	%eax, 16(%rsi)
	movq	8(%r8), %rax
	movq	%rax, 8(%rsi)
	xorl	%eax, %eax
	testq	%r11, %r11
	jne	.L33
	jmp	.L31
	.p2align 4,,10
	.p2align 3
.L32:
	leaq	1(%rdx), %rcx
	cmpq	%r9, %rcx
	je	.L31
.L40:
	movq	%rdx, %rax
	movq	%rcx, %rdx
.L33:
	leaq	(%rax,%rax,4), %rax
	leaq	(%r10,%rax,8), %rax
	cmpq	%rax, %r8
	jne	.L32
	leaq	1(%rdx), %rcx
	movl	$0, 16(%r8)
	cmpq	%r9, %rcx
	jne	.L40
	.p2align 4,,10
	.p2align 3
.L31:
	movq	24(%r8), %rdx
	movq	%rsi, %rax
	testq	%rdx, %rdx
	movq	%rdx, 24(%rsi)
	je	.L34
	movq	%rsi, 32(%rdx)
	jmp	.L35
	.cfi_endproc
.LFE6:
	.size	free_mem_pages, .-free_mem_pages
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC2:
	.string	"Failed to initialize memory!"
	.text
	.p2align 4,,15
	.globl	mm_init
	.type	mm_init, @function
mm_init:
.LFB7:
	.cfi_startproc
	salq	$10, %rdx
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rsi, kernel_pml4(%rip)
	movq	%rdx, total_memory(%rip)
	andq	$-4096, %rdx
	movq	%rdi, %r8
	movq	%rdx, %rbx
	movq	%rdx, last_free_addr(%rip)
	movq	%rdi, block_node_array(%rip)
	subq	%rdi, %rbx
	movq	%rbx, %r9
	shrq	$12, %r9
	leaq	(%r9,%r9,4), %rax
	movq	%r9, all_nodes(%rip)
	salq	$3, %rax
	leaq	4095(%rdi,%rax), %r11
	subq	%rax, %rbx
	movq	%rbx, available_free_memory(%rip)
	andq	$-4096, %r11
	testq	%r9, %r9
	movq	%r11, first_free_addr(%rip)
	movq	%r11, kernel_end_addr(%rip)
	je	.L48
	leaq	1(%r9), %r10
	movl	$1, %ecx
	xorl	%eax, %eax
	jmp	.L49
	.p2align 4,,10
	.p2align 3
.L57:
	movq	%rcx, %rax
	movq	%rsi, %rcx
.L49:
	leaq	1(%rcx), %rsi
	leaq	(%rax,%rax,4), %rax
	cmpq	%r10, %rsi
	movl	$0, 16(%rdi,%rax,8)
	jne	.L57
	movl	16(%rdi), %ecx
	shrq	$12, %rbx
	addq	$40, %rdi
	movl	$1, %eax
	testl	%ecx, %ecx
	jne	.L52
	jmp	.L50
	.p2align 4,,10
	.p2align 3
.L55:
	movl	16(%rdi), %ecx
	movq	%rdi, %r8
	addq	$1, %rax
	addq	$40, %rdi
	testl	%ecx, %ecx
	je	.L50
.L52:
	cmpq	%r9, %rax
	jne	.L55
.L56:
	movq	$0, blist(%rip)
.L53:
	movl	$.LC2, %edi
	call	panic
	movq	blist(%rip), %r8
	movq	first_free_addr(%rip), %r11
	movq	last_free_addr(%rip), %rdx
	.p2align 4,,10
	.p2align 3
.L54:
	movq	%r11, (%r8)
	movq	%rdx, 8(%r8)
	movl	%ebx, 16(%r8)
	movl	$1, 20(%r8)
	movq	$0, 24(%r8)
	movq	$0, 32(%r8)
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	.cfi_restore 3
	jmp	map_iomem
	.p2align 4,,10
	.p2align 3
.L50:
	.cfi_restore_state
	testq	%r8, %r8
	movq	%r8, blist(%rip)
	jne	.L54
	jmp	.L53
	.p2align 4,,10
	.p2align 3
.L48:
	shrq	$12, %rbx
	jmp	.L56
	.cfi_endproc
.LFE7:
	.size	mm_init, .-mm_init
	.comm	kernel_pml4,8,16
	.comm	kernel_end_addr,8,16
	.local	blist
	.comm	blist,8,16
	.local	block_node_array
	.comm	block_node_array,8,16
	.local	all_nodes
	.comm	all_nodes,8,16
	.local	total_memory
	.comm	total_memory,8,16
	.local	first_free_addr
	.comm	first_free_addr,8,16
	.local	last_free_addr
	.comm	last_free_addr,8,16
	.local	available_free_memory
	.comm	available_free_memory,8,16
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

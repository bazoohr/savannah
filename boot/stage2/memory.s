	.file	"memory.c"
	.comm	kernel_end_addr,8,8
	.comm	kernel_pml4,8,8
	.local	block_node_array
	.comm	block_node_array,8,8
	.local	blist
	.comm	blist,8,8
	.local	all_nodes
	.comm	all_nodes,8,8
	.local	first_free_addr
	.comm	first_free_addr,8,8
	.local	last_free_addr
	.comm	last_free_addr,8,8
	.local	total_memory
	.comm	total_memory,8,8
	.local	available_free_memory
	.comm	available_free_memory,8,8
	.text
	.type	init_block_node, @function
init_block_node:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	available_free_memory(%rip), %rax
	shrq	$12, %rax
	movq	%rax, all_nodes(%rip)
	movq	first_free_addr(%rip), %rax
	movq	%rax, block_node_array(%rip)
	movq	available_free_memory(%rip), %rcx
	movq	all_nodes(%rip), %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	movq	%rcx, %rdx
	subq	%rax, %rdx
	movq	%rdx, %rax
	movq	%rax, available_free_memory(%rip)
	movq	all_nodes(%rip), %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	movq	%rax, %rdx
	movq	first_free_addr(%rip), %rax
	addq	%rdx, %rax
	movq	%rax, first_free_addr(%rip)
	movq	first_free_addr(%rip), %rax
	addq	$4095, %rax
	andq	$-4096, %rax
	movq	%rax, first_free_addr(%rip)
	movq	first_free_addr(%rip), %rax
	movq	%rax, kernel_end_addr(%rip)
	movl	$0, -4(%rbp)
	jmp	.L2
.L3:
	movq	block_node_array(%rip), %rcx
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movl	$0, 16(%rax)
	addl	$1, -4(%rbp)
.L2:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	all_nodes(%rip), %rax
	cmpq	%rax, %rdx
	jb	.L3
	leave
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	init_block_node, .-init_block_node
	.type	alloc_block_node, @function
alloc_block_node:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	$0, -4(%rbp)
	jmp	.L5
.L8:
	movq	block_node_array(%rip), %rcx
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movl	16(%rax), %eax
	testl	%eax, %eax
	jne	.L6
	movq	block_node_array(%rip), %rcx
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	jmp	.L7
.L6:
	addl	$1, -4(%rbp)
.L5:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	all_nodes(%rip), %rax
	cmpq	%rax, %rdx
	jb	.L8
	movl	$0, %eax
.L7:
	leave
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	alloc_block_node, .-alloc_block_node
	.type	free_block_node, @function
free_block_node:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$24, %rsp
	movq	%rdi, -24(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L10
.L12:
	movq	block_node_array(%rip), %rcx
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	cmpq	-24(%rbp), %rax
	jne	.L11
	movq	block_node_array(%rip), %rcx
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movl	$0, 16(%rax)
.L11:
	addl	$1, -4(%rbp)
.L10:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	all_nodes(%rip), %rax
	cmpq	%rax, %rdx
	jb	.L12
	leave
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	free_block_node, .-free_block_node
	.section	.rodata
.LC0:
	.string	"Failed to initialize memory!"
	.text
	.type	init_block_list, @function
init_block_list:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	available_free_memory(%rip), %rax
	shrq	$12, %rax
	movq	%rax, -8(%rbp)
	call	alloc_block_node
	movq	%rax, blist(%rip)
	movq	blist(%rip), %rax
	testq	%rax, %rax
	jne	.L14
	movl	$.LC0, %edi
	call	panic
.L14:
	movq	blist(%rip), %rax
	movq	first_free_addr(%rip), %rdx
	movq	%rdx, (%rax)
	movq	blist(%rip), %rax
	movq	last_free_addr(%rip), %rdx
	movq	%rdx, 8(%rax)
	movq	blist(%rip), %rax
	movq	-8(%rbp), %rdx
	movl	%edx, 16(%rax)
	movq	blist(%rip), %rax
	movl	$1, 20(%rax)
	movq	blist(%rip), %rax
	movq	$0, 24(%rax)
	movq	blist(%rip), %rax
	movq	$0, 32(%rax)
	leave
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	init_block_list, .-init_block_list
	.globl	alloc_mem_pages
	.type	alloc_mem_pages, @function
alloc_mem_pages:
.LFB4:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$40, %rsp
	movq	%rdi, -40(%rbp)
	movq	$0, -16(%rbp)
	movq	blist(%rip), %rax
	movq	%rax, -8(%rbp)
	jmp	.L16
.L21:
	movq	-8(%rbp), %rax
	movl	20(%rax), %eax
	testl	%eax, %eax
	je	.L17
	movq	-8(%rbp), %rax
	movl	16(%rax), %eax
	cltq
	cmpq	-40(%rbp), %rax
	jae	.L18
.L17:
	movq	-8(%rbp), %rax
	movq	24(%rax), %rax
	movq	%rax, -8(%rbp)
	jmp	.L16
.L18:
	movq	-8(%rbp), %rax
	movl	16(%rax), %eax
	cltq
	cmpq	-40(%rbp), %rax
	jne	.L19
	movq	-8(%rbp), %rax
	movl	$0, 20(%rax)
	movq	-8(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, -16(%rbp)
	jmp	.L20
.L19:
	call	alloc_block_node
	movq	%rax, -24(%rbp)
	movq	-8(%rbp), %rax
	movq	(%rax), %rax
	movq	-40(%rbp), %rdx
	salq	$12, %rdx
	addq	%rax, %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, (%rax)
	movq	-8(%rbp), %rax
	movq	8(%rax), %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, 8(%rax)
	movq	-8(%rbp), %rax
	movl	16(%rax), %eax
	movl	%eax, %edx
	movq	-40(%rbp), %rax
	movl	%edx, %ecx
	subl	%eax, %ecx
	movl	%ecx, %eax
	movl	%eax, %edx
	movq	-24(%rbp), %rax
	movl	%edx, 16(%rax)
	movq	-24(%rbp), %rax
	movq	-8(%rbp), %rdx
	movq	%rdx, 32(%rax)
	movq	-8(%rbp), %rax
	movq	24(%rax), %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, 24(%rax)
	movq	-24(%rbp), %rax
	movl	$1, 20(%rax)
	movq	-40(%rbp), %rax
	movl	%eax, %edx
	movq	-8(%rbp), %rax
	movl	%edx, 16(%rax)
	movq	-24(%rbp), %rax
	movq	(%rax), %rdx
	movq	-8(%rbp), %rax
	movq	%rdx, 8(%rax)
	movq	-8(%rbp), %rax
	movl	$0, 20(%rax)
	movq	-8(%rbp), %rax
	movq	-24(%rbp), %rdx
	movq	%rdx, 24(%rax)
	movq	-8(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, -16(%rbp)
	jmp	.L20
.L16:
	cmpq	$0, -8(%rbp)
	jne	.L21
.L20:
	movq	-16(%rbp), %rax
	leave
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	alloc_mem_pages, .-alloc_mem_pages
	.section	.rodata
	.align 8
.LC1:
	.string	"=============== MEMORY LAYOUT ===============\n"
	.align 8
.LC2:
	.string	"block address = %x pages = %x free = %d\n"
	.text
	.globl	get_memory_status
	.type	get_memory_status, @function
get_memory_status:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	blist(%rip), %rax
	movq	%rax, -8(%rbp)
	movl	$11, %esi
	movl	$.LC1, %edi
	movl	$0, %eax
	call	cprintk
	jmp	.L23
.L26:
	movq	$0, -16(%rbp)
	movq	$0, -16(%rbp)
	jmp	.L24
.L25:
	addq	$1, -16(%rbp)
.L24:
	cmpq	$9999899, -16(%rbp)
	jbe	.L25
	movq	-8(%rbp), %rax
	movl	20(%rax), %ecx
	movq	-8(%rbp), %rax
	movl	16(%rax), %edx
	movq	-8(%rbp), %rax
	movq	(%rax), %rax
	movl	%ecx, %r8d
	movl	%edx, %ecx
	movq	%rax, %rdx
	movl	$11, %esi
	movl	$.LC2, %edi
	movl	$0, %eax
	call	cprintk
	movq	-8(%rbp), %rax
	movq	24(%rax), %rax
	movq	%rax, -8(%rbp)
.L23:
	cmpq	$0, -8(%rbp)
	jne	.L26
	leave
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	get_memory_status, .-get_memory_status
	.globl	free_mem_pages
	.type	free_mem_pages, @function
free_mem_pages:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$24, %rsp
	movq	%rdi, -24(%rbp)
	movq	blist(%rip), %rax
	movq	%rax, -8(%rbp)
	jmp	.L28
.L34:
	movq	-8(%rbp), %rax
	movq	(%rax), %rax
	cmpq	-24(%rbp), %rax
	jne	.L29
	movq	-8(%rbp), %rax
	movl	20(%rax), %eax
	testl	%eax, %eax
	jne	.L35
	movq	-8(%rbp), %rax
	movq	32(%rax), %rax
	testq	%rax, %rax
	je	.L31
	movq	-8(%rbp), %rax
	movq	32(%rax), %rax
	movl	20(%rax), %eax
	cmpl	$1, %eax
	jne	.L31
	movq	-8(%rbp), %rax
	movq	32(%rax), %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	16(%rax), %edx
	movq	-8(%rbp), %rax
	movq	24(%rax), %rax
	movl	16(%rax), %eax
	addl	%eax, %edx
	movq	-8(%rbp), %rax
	movl	%edx, 16(%rax)
	movq	-8(%rbp), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rdx
	movq	-8(%rbp), %rax
	movq	%rdx, 8(%rax)
	movq	-8(%rbp), %rax
	movq	24(%rax), %rax
	movq	%rax, %rdi
	call	free_block_node
	movq	-8(%rbp), %rax
	movq	24(%rax), %rax
	movq	24(%rax), %rdx
	movq	-8(%rbp), %rax
	movq	%rdx, 24(%rax)
	movq	-8(%rbp), %rax
	movq	24(%rax), %rax
	testq	%rax, %rax
	je	.L31
	movq	-8(%rbp), %rax
	movq	24(%rax), %rax
	movq	-8(%rbp), %rdx
	movq	%rdx, 32(%rax)
.L31:
	movq	-8(%rbp), %rax
	movq	24(%rax), %rax
	testq	%rax, %rax
	je	.L32
	movq	-8(%rbp), %rax
	movq	24(%rax), %rax
	movl	20(%rax), %eax
	cmpl	$1, %eax
	jne	.L32
	movq	-8(%rbp), %rax
	movl	16(%rax), %edx
	movq	-8(%rbp), %rax
	movq	24(%rax), %rax
	movl	16(%rax), %eax
	addl	%eax, %edx
	movq	-8(%rbp), %rax
	movl	%edx, 16(%rax)
	movq	-8(%rbp), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rdx
	movq	-8(%rbp), %rax
	movq	%rdx, 8(%rax)
	movq	-8(%rbp), %rax
	movq	24(%rax), %rax
	movq	%rax, %rdi
	call	free_block_node
	movq	-8(%rbp), %rax
	movq	24(%rax), %rax
	movq	24(%rax), %rdx
	movq	-8(%rbp), %rax
	movq	%rdx, 24(%rax)
	movq	-8(%rbp), %rax
	movq	24(%rax), %rax
	testq	%rax, %rax
	je	.L32
	movq	-8(%rbp), %rax
	movq	24(%rax), %rax
	movq	-8(%rbp), %rdx
	movq	%rdx, 32(%rax)
.L32:
	movq	-8(%rbp), %rax
	movl	$1, 20(%rax)
.L29:
	movq	-8(%rbp), %rax
	movq	24(%rax), %rax
	movq	%rax, -8(%rbp)
.L28:
	cmpq	$0, -8(%rbp)
	jne	.L34
	jmp	.L27
.L35:
	nop
.L27:
	leave
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	free_mem_pages, .-free_mem_pages
	.globl	mm_init
	.type	mm_init, @function
mm_init:
.LFB7:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, kernel_pml4(%rip)
	movq	-24(%rbp), %rax
	salq	$10, %rax
	movq	%rax, total_memory(%rip)
	movq	-8(%rbp), %rax
	movq	%rax, first_free_addr(%rip)
	movq	total_memory(%rip), %rax
	andq	$-4096, %rax
	movq	%rax, last_free_addr(%rip)
	movq	last_free_addr(%rip), %rdx
	movq	first_free_addr(%rip), %rax
	movq	%rdx, %rcx
	subq	%rax, %rcx
	movq	%rcx, %rax
	movq	%rax, available_free_memory(%rip)
	call	init_block_node
	call	init_block_list
	call	map_iomem
	leave
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	mm_init, .-mm_init
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

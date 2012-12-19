	.file	"loader.c"
	.text
	.p2align 4,,15
	.globl	cpuid
	.type	cpuid, @function
cpuid:
.LFB0:
	.cfi_startproc
	subl	$16, %esp
	.cfi_def_cfa_offset 20
	movl	%esi, 4(%esp)
	movl	20(%esp), %eax
	movl	%edi, 8(%esp)
	movl	36(%esp), %esi
	.cfi_offset 7, -12
	.cfi_offset 6, -16
	movl	%ebp, 12(%esp)
	movl	24(%esp), %edi
	movl	%ebx, (%esp)
	movl	28(%esp), %ebp
	.cfi_offset 3, -20
	.cfi_offset 5, -8
#APP
# 23 "loader.c" 1
	cpuid
# 0 "" 2
#NO_APP
	testl	%edi, %edi
	je	.L2
	movl	%eax, (%edi)
.L2:
	testl	%ebp, %ebp
	je	.L3
	movl	%ebx, 0(%ebp)
.L3:
	movl	32(%esp), %eax
	testl	%eax, %eax
	je	.L4
	movl	32(%esp), %eax
	movl	%ecx, (%eax)
.L4:
	testl	%esi, %esi
	je	.L1
	movl	%edx, (%esi)
.L1:
	movl	(%esp), %ebx
	movl	4(%esp), %esi
	movl	8(%esp), %edi
	movl	12(%esp), %ebp
	addl	$16, %esp
	.cfi_def_cfa_offset 4
	.cfi_restore 5
	.cfi_restore 7
	.cfi_restore 6
	.cfi_restore 3
	ret
	.cfi_endproc
.LFE0:
	.size	cpuid, .-cpuid
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align 4
.LC0:
	.string	"ERROR: Your system is not supporting long mode!\n"
	.align 4
.LC1:
	.string	"ERROR: Your system does not support X87 and media instructions.\n"
	.align 4
.LC2:
	.string	"ERROR: Size of memory is not known!\n"
	.align 4
.LC3:
	.string	"ERROR: Grub failed to load all needed modules. %d modules loaded, but 3 were needed!\n"
	.align 4
.LC4:
	.string	"ERROR: loaded module overlaps with VMM start address\n"
	.align 4
.LC5:
	.string	"ERROR: loaded module overlaps with kernel start address\n"
	.align 4
.LC6:
	.string	"ERROR: Kernel is not loaded as multiboot!\n"
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC7:
	.string	"Unexpected Kernel Header!\n"
	.text
	.p2align 4,,15
	.globl	boot_loader
	.type	boot_loader, @function
boot_loader:
.LFB2:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	pushl	%edi
	.cfi_def_cfa_offset 12
	.cfi_offset 7, -12
	pushl	%esi
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushl	%ebx
	.cfi_def_cfa_offset 20
	.cfi_offset 3, -20
	subl	$60, %esp
	.cfi_def_cfa_offset 80
	movl	84(%esp), %ebx
	call	clrscr
	call	has_long_mode
	testl	%eax, %eax
	je	.L25
	.p2align 4,,6
	call	has_x87_and_media_support
	cmpl	$1, %eax
	.p2align 4,,3
	je	.L26
.L9:
	movl	$.LC1, (%esp)
	call	printf
	call	halt
.L10:
	call	has_1GBpage
	movl	(%ebx), %eax
	testb	$1, %al
	je	.L11
	movl	8(%ebx), %edx
	movl	$0, 36(%esp)
	addl	$1, %edx
	movl	%edx, 32(%esp)
.L12:
	testb	$8, %al
	je	.L13
	movl	20(%ebx), %eax
	cmpl	$3, %eax
	je	.L14
	movl	%eax, 4(%esp)
	movl	$.LC3, (%esp)
	call	printf
	call	halt
.L14:
	movl	24(%ebx), %ebx
	cmpl	$2097152, 4(%ebx)
	movl	(%ebx), %esi
	ja	.L27
.L15:
	cmpl	$2097152, 20(%ebx)
	movl	16(%ebx), %edi
	ja	.L28
.L16:
	movl	$512, 8(%esp)
	movl	%edi, 4(%esp)
	movl	$651264, (%esp)
	call	memcpy
	movl	32(%ebx), %eax
	cmpl	$2097152, 36(%ebx)
	movl	%eax, 24(%esp)
	ja	.L17
	movl	%esi, 40(%esp)
	movl	24(%esp), %esi
	movl	$0, 44(%esp)
.L18:
	movzwl	56(%eax), %ebp
	addl	32(%eax), %esi
	cmpl	$4, %ebp
	je	.L19
	movl	$.LC7, (%esp)
	subl	$1, %ebp
	movl	%esi, %ebx
	call	printf
	call	halt
	movl	16(%esi), %eax
	testl	%ebp, %ebp
	movl	%eax, 28(%esp)
	jle	.L21
.L20:
	xorl	%edi, %edi
	jmp	.L23
	.p2align 4,,7
	.p2align 3
.L22:
	addl	$1, %edi
	cmpl	%edi, %ebp
	leal	56(%esi), %ebx
	jle	.L29
.L24:
	movl	%ebx, %esi
.L23:
	movl	40(%esi), %eax
	movl	8(%esi), %ecx
	movl	16(%esi), %edx
	testl	%eax, %eax
	je	.L22
	addl	24(%esp), %ecx
	addl	$1, %edi
	movl	%eax, 8(%esp)
	leal	56(%esi), %ebx
	movl	%edx, 4(%esp)
	movl	%ecx, (%esp)
	call	bcopy
	cmpl	%edi, %ebp
	jg	.L24
.L29:
	movl	72(%esi), %eax
.L21:
	movl	40(%ebx), %edx
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	movl	%edx, 8(%esp)
	call	memset
	movl	32(%esp), %edx
	movl	36(%esp), %ecx
	movl	40(%ebx), %eax
	addl	16(%ebx), %eax
	movl	%edx, kargs
	movl	40(%esp), %edx
	movl	%ecx, kargs+4
	movl	44(%esp), %ecx
	movl	%eax, kargs+16
	movl	$0, kargs+20
	movl	%edx, kargs+24
	movl	%ecx, kargs+28
	movl	28(%esp), %ecx
	movl	$kargs, 80(%esp)
	addl	$60, %esp
	.cfi_remember_state
	.cfi_def_cfa_offset 20
	popl	%ebx
	.cfi_def_cfa_offset 16
	.cfi_restore 3
	popl	%esi
	.cfi_def_cfa_offset 12
	.cfi_restore 6
	popl	%edi
	.cfi_def_cfa_offset 8
	.cfi_restore 7
	popl	%ebp
	.cfi_def_cfa_offset 4
	.cfi_restore 5
	jmp	*%ecx
	.p2align 4,,7
	.p2align 3
.L19:
	.cfi_restore_state
	movl	16(%esi), %edx
	movl	$3, %ebp
	movl	%edx, 28(%esp)
	jmp	.L20
	.p2align 4,,7
	.p2align 3
.L13:
	movl	$.LC6, (%esp)
	call	printf
	call	halt
	xorl	%eax, %eax
	movl	$0, 24(%esp)
	movl	24(%esp), %esi
	movl	$0, 40(%esp)
	movl	$0, 44(%esp)
	jmp	.L18
	.p2align 4,,7
	.p2align 3
.L11:
	movl	$.LC2, (%esp)
	call	printf
	call	halt
	movl	(%ebx), %eax
	movl	$0, 32(%esp)
	movl	$0, 36(%esp)
	jmp	.L12
	.p2align 4,,7
	.p2align 3
.L25:
	movl	$.LC0, (%esp)
	call	printf
	call	halt
	call	has_x87_and_media_support
	cmpl	$1, %eax
	.p2align 4,,2
	jne	.L9
.L26:
	.p2align 4,,5
	call	enable_x87_and_media
	.p2align 4,,5
	jmp	.L10
	.p2align 4,,7
	.p2align 3
.L17:
	movl	$.LC5, (%esp)
	call	printf
	call	halt
	movl	24(%esp), %eax
	movl	%esi, 40(%esp)
	movl	$0, 44(%esp)
	movl	%eax, %esi
	jmp	.L18
	.p2align 4,,7
	.p2align 3
.L28:
	movl	$.LC4, (%esp)
	call	printf
	call	halt
	jmp	.L16
	.p2align 4,,7
	.p2align 3
.L27:
	movl	$.LC4, (%esp)
	call	printf
	call	halt
	jmp	.L15
	.cfi_endproc
.LFE2:
	.size	boot_loader, .-boot_loader
	.local	kargs
	.comm	kargs,32,32
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

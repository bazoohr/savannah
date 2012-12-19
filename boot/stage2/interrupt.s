	.file	"interrupt.c"
	.text
	.p2align 4,,15
	.globl	interrupt_init
	.type	interrupt_init, @function
interrupt_init:
.LFB4:
	.cfi_startproc
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	xorl	%edx, %edx
	pushq	%r13
	.cfi_def_cfa_offset 24
	.cfi_offset 13, -24
	pushq	%r12
	.cfi_def_cfa_offset 32
	.cfi_offset 12, -32
	movl	$reserved, %r12d
	movl	%r12d, %r13d
	shrq	$16, %r12
	pushq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	movq	%r13, %rax
	movq	%r12, %r14
	movq	%r12, %r8
	movq	%r12, %rdi
	movzbl	%ah, %ebp
	shrq	$16, %r14
	movq	%r12, %rax
	shrq	$24, %r8
	shrq	$32, %rdi
	movq	%r12, %rsi
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	movzbl	%r13b, %r11d
	movzbl	%r12b, %r10d
	movzbl	%ah, %ebx
	movzbl	%r14b, %r9d
	andl	$255, %r8d
	andl	$255, %edi
	shrq	$40, %rsi
	.p2align 4,,10
	.p2align 3
.L2:
	movq	%rdx, %rcx
	addq	$1, %rdx
	salq	$4, %rcx
	cmpq	$256, %rdx
	movb	%r11b, idt(%rcx)
	movb	%bpl, idt+1(%rcx)
	movb	$16, idt+2(%rcx)
	movb	$0, idt+3(%rcx)
	movb	$0, idt+4(%rcx)
	movb	$-113, idt+5(%rcx)
	movb	%r10b, idt+6(%rcx)
	movb	%bl, idt+7(%rcx)
	movb	%r9b, idt+8(%rcx)
	movb	%r8b, idt+9(%rcx)
	movb	%dil, idt+10(%rcx)
	movb	%sil, idt+11(%rcx)
	movb	$0, idt+12(%rcx)
	movb	$0, idt+13(%rcx)
	movb	$0, idt+14(%rcx)
	movb	$0, idt+15(%rcx)
	jne	.L2
	movl	$divide_error, %eax
	movw	$16, idt+2(%rip)
	movb	$0, idt+4(%rip)
	movw	%ax, idt(%rip)
	movq	%rax, %rdx
	shrq	$32, %rax
	shrq	$16, %rdx
	movl	%eax, idt+8(%rip)
	movl	$debug, %eax
	movw	%dx, idt+6(%rip)
	movw	%ax, idt+16(%rip)
	movq	%rax, %rdx
	shrq	$32, %rax
	shrq	$16, %rdx
	movb	$-113, idt+5(%rip)
	movl	%eax, idt+24(%rip)
	movl	$nmi, %eax
	movw	%dx, idt+22(%rip)
	movw	%ax, idt+32(%rip)
	movq	%rax, %rdx
	shrq	$32, %rax
	shrq	$16, %rdx
	movl	%eax, idt+40(%rip)
	movl	$breakpoint, %eax
	movw	%dx, idt+38(%rip)
	movw	%ax, idt+48(%rip)
	movq	%rax, %rdx
	shrq	$32, %rax
	shrq	$16, %rdx
	movl	$0, idt+12(%rip)
	movl	%eax, idt+56(%rip)
	movl	$overflow, %eax
	movw	%dx, idt+54(%rip)
	movq	%rax, %rdx
	movw	%ax, idt+64(%rip)
	shrq	$32, %rax
	shrq	$16, %rdx
	movw	$16, idt+18(%rip)
	movb	$0, idt+20(%rip)
	movw	%dx, idt+70(%rip)
	movb	$-113, idt+21(%rip)
	movl	$idtr, %edi
	movl	$0, idt+28(%rip)
	movw	$16, idt+34(%rip)
	movb	$0, idt+36(%rip)
	movb	$-113, idt+37(%rip)
	movl	$0, idt+44(%rip)
	movw	$16, idt+50(%rip)
	movb	$0, idt+52(%rip)
	movb	$-113, idt+53(%rip)
	movl	$0, idt+60(%rip)
	movw	$16, idt+66(%rip)
	movb	$0, idt+68(%rip)
	movb	$-113, idt+69(%rip)
	movl	%eax, idt+72(%rip)
	movl	$bound_range, %eax
	movw	%r13w, idt+144(%rip)
	movw	%ax, idt+80(%rip)
	movq	%rax, %rdx
	shrq	$32, %rax
	shrq	$16, %rdx
	movl	%eax, idt+88(%rip)
	movl	$invalid_opcode, %eax
	movw	%dx, idt+86(%rip)
	movw	%ax, idt+96(%rip)
	movq	%rax, %rdx
	shrq	$32, %rax
	shrq	$16, %rdx
	movl	$0, idt+76(%rip)
	movl	%eax, idt+104(%rip)
	movl	$device_not_available, %eax
	movw	%dx, idt+102(%rip)
	movw	%ax, idt+112(%rip)
	movq	%rax, %rdx
	shrq	$32, %rax
	shrq	$16, %rdx
	movl	%eax, idt+120(%rip)
	movl	$double_fault, %eax
	movw	%dx, idt+118(%rip)
	movw	%ax, idt+128(%rip)
	movq	%rax, %rdx
	shrq	$32, %rax
	shrq	$16, %rdx
	movw	$16, idt+82(%rip)
	movl	%eax, idt+136(%rip)
	movl	$invalid_tss, %eax
	movw	%dx, idt+134(%rip)
	movb	$0, idt+84(%rip)
	movq	%rax, %rdx
	movb	$-113, idt+85(%rip)
	movl	$0, idt+92(%rip)
	movw	$16, idt+98(%rip)
	shrq	$16, %rdx
	movb	$0, idt+100(%rip)
	movb	$-113, idt+101(%rip)
	movl	$0, idt+108(%rip)
	movw	$16, idt+114(%rip)
	movb	$0, idt+116(%rip)
	movb	$-113, idt+117(%rip)
	movl	$0, idt+124(%rip)
	movw	$16, idt+130(%rip)
	movb	$0, idt+132(%rip)
	movb	$-113, idt+133(%rip)
	movl	$0, idt+140(%rip)
	movw	$16, idt+146(%rip)
	movb	$0, idt+148(%rip)
	movb	$-113, idt+149(%rip)
	movw	%r12w, idt+150(%rip)
	movw	%ax, idt+160(%rip)
	shrq	$32, %rax
	movl	%eax, idt+168(%rip)
	movl	$segment_not_present, %eax
	movw	%dx, idt+166(%rip)
	movw	%ax, idt+176(%rip)
	movq	%rax, %rdx
	shrq	$32, %rax
	shrq	$16, %rdx
	movl	%eax, idt+184(%rip)
	movl	$stack_exception, %eax
	movw	%dx, idt+182(%rip)
	movw	%ax, idt+192(%rip)
	movq	%rax, %rdx
	shrq	$32, %rax
	shrq	$16, %rdx
	movl	%r14d, idt+152(%rip)
	movl	%eax, idt+200(%rip)
	movl	$general_protection, %eax
	movw	%dx, idt+198(%rip)
	movw	%ax, idt+208(%rip)
	movq	%rax, %rdx
	shrq	$32, %rax
	shrq	$16, %rdx
	movl	%eax, idt+216(%rip)
	movl	$page_fault, %eax
	movw	%dx, idt+214(%rip)
	movw	%ax, idt+224(%rip)
	movq	%rax, %rdx
	shrq	$32, %rax
	shrq	$16, %rdx
	movl	$0, idt+156(%rip)
	movw	$16, idt+162(%rip)
	movb	$0, idt+164(%rip)
	movb	$-113, idt+165(%rip)
	movl	$0, idt+172(%rip)
	movw	$16, idt+178(%rip)
	movb	$0, idt+180(%rip)
	movb	$-113, idt+181(%rip)
	movl	$0, idt+188(%rip)
	movw	$16, idt+194(%rip)
	movb	$0, idt+196(%rip)
	movb	$-113, idt+197(%rip)
	movl	$0, idt+204(%rip)
	movw	$16, idt+210(%rip)
	movb	$0, idt+212(%rip)
	movb	$-113, idt+213(%rip)
	movl	$0, idt+220(%rip)
	movw	$16, idt+226(%rip)
	movb	$0, idt+228(%rip)
	movb	$-113, idt+229(%rip)
	movl	%eax, idt+232(%rip)
	movl	$fp_exception, %eax
	movw	%dx, idt+230(%rip)
	movw	%ax, idt+256(%rip)
	movq	%rax, %rdx
	shrq	$32, %rax
	shrq	$16, %rdx
	movl	$0, idt+236(%rip)
	movl	%eax, idt+264(%rip)
	movl	$alignment_check, %eax
	movw	%dx, idt+262(%rip)
	movw	%ax, idt+272(%rip)
	movq	%rax, %rdx
	shrq	$32, %rax
	shrq	$16, %rdx
	movl	%eax, idt+280(%rip)
	movl	$machine_check, %eax
	movw	%dx, idt+278(%rip)
	movq	%rax, %rdx
	movw	%ax, idt+288(%rip)
	shrq	$16, %rdx
	shrq	$32, %rax
	movw	$16, idt+258(%rip)
	movl	%eax, idt+296(%rip)
	movl	$xmm, %eax
	movw	%dx, idt+294(%rip)
	movw	%ax, idt+304(%rip)
	movq	%rax, %rdx
	shrq	$32, %rax
	shrq	$16, %rdx
	movl	%eax, idt+312(%rip)
	movl	$system_call, %eax
	movw	%dx, idt+310(%rip)
	movw	%ax, idt+2048(%rip)
	movq	%rax, %rdx
	movb	$0, idt+260(%rip)
	movb	$-113, idt+261(%rip)
	shrq	$32, %rax
	movl	$0, idt+268(%rip)
	movw	$16, idt+274(%rip)
	shrq	$16, %rdx
	movb	$0, idt+276(%rip)
	movb	$-113, idt+277(%rip)
	movl	$0, idt+284(%rip)
	movw	$16, idt+290(%rip)
	movb	$0, idt+292(%rip)
	movb	$-113, idt+293(%rip)
	movl	$0, idt+300(%rip)
	movw	$16, idt+306(%rip)
	movb	$0, idt+308(%rip)
	movb	$-113, idt+309(%rip)
	movl	$0, idt+316(%rip)
	movw	$16, idt+2050(%rip)
	movb	$0, idt+2052(%rip)
	movl	%eax, idt+2056(%rip)
	movl	$timer_handler, %eax
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
	movw	%dx, idt+2054(%rip)
	movw	%ax, idt+512(%rip)
	movq	%rax, %rdx
	popq	%r14
	.cfi_def_cfa_offset 8
	.cfi_restore 14
	shrq	$32, %rax
	shrq	$16, %rdx
	movb	$-17, idt+2053(%rip)
	movl	%eax, idt+520(%rip)
	movl	$kbd_handler, %eax
	movw	%dx, idt+518(%rip)
	movq	%rax, %rdx
	movw	%ax, idt+528(%rip)
	shrq	$32, %rax
	shrq	$16, %rdx
	movl	$0, idt+2060(%rip)
	movw	$16, idt+514(%rip)
	movb	$0, idt+516(%rip)
	movb	$-114, idt+517(%rip)
	movl	$0, idt+524(%rip)
	movw	$16, idt+530(%rip)
	movb	$0, idt+532(%rip)
	movb	$-114, idt+533(%rip)
	movw	%dx, idt+534(%rip)
	movl	%eax, idt+536(%rip)
	movl	$0, idt+540(%rip)
	movw	$4095, idtr(%rip)
	movq	$idt, idtr+2(%rip)
	jmp	lidt
	.cfi_endproc
.LFE4:
	.size	interrupt_init, .-interrupt_init
	.comm	__packed,216,32
	.local	idt
	.comm	idt,4096,16
	.local	idtr
	.comm	idtr,10,1
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

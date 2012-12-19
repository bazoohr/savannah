	.file	"keyboard.c"
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC0:
	.string	"Data can't be read from keyboard buffer\n"
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC1:
	.string	"Keyboard: %c\n"
	.text
	.p2align 4,,15
	.globl	kbd_proc_data
	.type	kbd_proc_data, @function
kbd_proc_data:
.LFB1:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	$100, %edi
	call	inb
	testb	$1, %al
	je	.L17
.L2:
	movl	$96, %edi
	call	inb
	movzbl	%al, %ebx
	cmpl	$224, %ebx
	je	.L3
	cmpb	$56, %al
	je	.L7
	jbe	.L18
	cmpb	$-86, %al
	.p2align 4,,7
	je	.L9
	.p2align 4,,9
	jbe	.L19
	cmpb	$-74, %al
	.p2align 4,,7
	je	.L9
	cmpb	$-72, %al
	.p2align 4,,7
	jne	.L4
	movl	$0, alt_pressed.1344(%rip)
.L3:
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	.cfi_restore 3
	jmp	lapic_eoi
	.p2align 4,,10
	.p2align 3
.L18:
	.cfi_restore_state
	cmpb	$42, %al
	je	.L6
	cmpb	$54, %al
	je	.L6
	cmpb	$29, %al
	.p2align 4,,5
	je	.L20
.L4:
	movl	alt_pressed.1344(%rip), %edx
	testl	%edx, %edx
	je	.L13
	cmpl	$83, %ebx
	je	.L21
.L13:
	testb	$-128, %bl
	jne	.L3
	movl	ctrl_pressed.1343(%rip), %eax
	movl	alt_pressed.1344(%rip), %edx
	movslq	%ebx, %rbx
	sall	$2, %edx
	addl	%eax, %eax
	orl	%edx, %eax
	orl	shift_pressed.1342(%rip), %eax
	leaq	(%rax,%rbx,8), %rax
	movl	keymap(,%rax,4), %edx
	movl	bufpos(%rip), %eax
	cmpl	$63, %eax
	leal	1(%rax), %ecx
	jg	.L22
.L14:
	cltq
	movl	$14, %esi
	movl	$.LC1, %edi
	movl	%edx, buffer(,%rax,4)
	xorl	%eax, %eax
	movl	%ecx, bufpos(%rip)
	call	cprintk
	jmp	.L3
	.p2align 4,,10
	.p2align 3
.L17:
	movl	$.LC0, %edi
	call	panic
	.p2align 4,,3
	jmp	.L2
	.p2align 4,,10
	.p2align 3
.L19:
	cmpb	$-99, %al
	jne	.L4
	popq	%rbx
	.cfi_remember_state
	.cfi_restore 3
	.cfi_def_cfa_offset 8
	movl	$0, ctrl_pressed.1343(%rip)
	jmp	lapic_eoi
	.p2align 4,,10
	.p2align 3
.L6:
	.cfi_restore_state
	popq	%rbx
	.cfi_remember_state
	.cfi_restore 3
	.cfi_def_cfa_offset 8
	movl	$1, shift_pressed.1342(%rip)
	jmp	lapic_eoi
	.p2align 4,,10
	.p2align 3
.L7:
	.cfi_restore_state
	popq	%rbx
	.cfi_remember_state
	.cfi_restore 3
	.cfi_def_cfa_offset 8
	movl	$1, alt_pressed.1344(%rip)
	jmp	lapic_eoi
	.p2align 4,,10
	.p2align 3
.L20:
	.cfi_restore_state
	popq	%rbx
	.cfi_remember_state
	.cfi_restore 3
	.cfi_def_cfa_offset 8
	movl	$1, ctrl_pressed.1343(%rip)
	jmp	lapic_eoi
	.p2align 4,,10
	.p2align 3
.L9:
	.cfi_restore_state
	popq	%rbx
	.cfi_remember_state
	.cfi_restore 3
	.cfi_def_cfa_offset 8
	movl	$0, shift_pressed.1342(%rip)
	jmp	lapic_eoi
	.p2align 4,,10
	.p2align 3
.L22:
	.cfi_restore_state
	movl	$1, %ecx
	xorl	%eax, %eax
	jmp	.L14
	.p2align 4,,10
	.p2align 3
.L21:
	movl	ctrl_pressed.1343(%rip), %eax
	testl	%eax, %eax
	je	.L13
	call	halt
	jmp	.L13
	.cfi_endproc
.LFE1:
	.size	kbd_proc_data, .-kbd_proc_data
	.p2align 4,,15
	.globl	kbd_init
	.type	kbd_init, @function
kbd_init:
.LFB2:
	.cfi_startproc
	movl	$1, %esi
	movl	$1, %edi
	jmp	ioapic_enable
	.cfi_endproc
.LFE2:
	.size	kbd_init, .-kbd_init
	.local	ctrl_pressed.1343
	.comm	ctrl_pressed.1343,4,16
	.local	alt_pressed.1344
	.comm	alt_pressed.1344,4,16
	.local	shift_pressed.1342
	.comm	shift_pressed.1342,4,16
	.section	.rodata
	.align 32
	.type	keymap, @object
	.size	keymap, 2688
keymap:
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	27
	.long	27
	.long	27
	.long	27
	.long	27
	.long	27
	.long	0
	.long	27
	.long	49
	.long	33
	.long	0
	.long	0
	.long	49
	.long	33
	.long	0
	.long	0
	.long	50
	.long	64
	.long	0
	.long	0
	.long	50
	.long	64
	.long	0
	.long	0
	.long	51
	.long	35
	.long	0
	.long	0
	.long	51
	.long	35
	.long	0
	.long	0
	.long	52
	.long	36
	.long	0
	.long	0
	.long	52
	.long	36
	.long	0
	.long	0
	.long	53
	.long	37
	.long	0
	.long	0
	.long	53
	.long	37
	.long	0
	.long	0
	.long	54
	.long	94
	.long	30
	.long	30
	.long	54
	.long	94
	.long	30
	.long	30
	.long	55
	.long	38
	.long	0
	.long	0
	.long	55
	.long	38
	.long	0
	.long	0
	.long	56
	.long	42
	.long	0
	.long	0
	.long	56
	.long	42
	.long	0
	.long	0
	.long	57
	.long	40
	.long	0
	.long	0
	.long	57
	.long	40
	.long	0
	.long	0
	.long	48
	.long	41
	.long	0
	.long	0
	.long	48
	.long	41
	.long	0
	.long	0
	.long	45
	.long	95
	.long	31
	.long	31
	.long	45
	.long	95
	.long	31
	.long	31
	.long	61
	.long	43
	.long	0
	.long	0
	.long	61
	.long	43
	.long	0
	.long	0
	.long	8
	.long	8
	.long	127
	.long	127
	.long	8
	.long	8
	.long	127
	.long	127
	.long	9
	.long	3840
	.long	0
	.long	0
	.long	9
	.long	3840
	.long	0
	.long	0
	.long	113
	.long	81
	.long	17
	.long	17
	.long	113
	.long	81
	.long	17
	.long	17
	.long	119
	.long	87
	.long	23
	.long	23
	.long	119
	.long	87
	.long	23
	.long	23
	.long	101
	.long	69
	.long	5
	.long	5
	.long	101
	.long	69
	.long	5
	.long	5
	.long	114
	.long	82
	.long	18
	.long	18
	.long	114
	.long	82
	.long	18
	.long	18
	.long	116
	.long	84
	.long	20
	.long	20
	.long	116
	.long	84
	.long	20
	.long	20
	.long	121
	.long	89
	.long	25
	.long	25
	.long	121
	.long	89
	.long	25
	.long	25
	.long	117
	.long	85
	.long	21
	.long	21
	.long	117
	.long	85
	.long	21
	.long	21
	.long	105
	.long	73
	.long	9
	.long	9
	.long	105
	.long	73
	.long	9
	.long	9
	.long	111
	.long	79
	.long	15
	.long	15
	.long	111
	.long	79
	.long	15
	.long	15
	.long	112
	.long	80
	.long	16
	.long	16
	.long	112
	.long	80
	.long	16
	.long	16
	.long	91
	.long	123
	.long	27
	.long	27
	.long	91
	.long	123
	.long	27
	.long	27
	.long	93
	.long	125
	.long	29
	.long	29
	.long	93
	.long	125
	.long	29
	.long	29
	.long	10
	.long	10
	.long	10
	.long	10
	.long	10
	.long	10
	.long	10
	.long	10
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	97
	.long	65
	.long	1
	.long	1
	.long	97
	.long	65
	.long	1
	.long	1
	.long	115
	.long	83
	.long	19
	.long	19
	.long	115
	.long	83
	.long	19
	.long	19
	.long	100
	.long	68
	.long	4
	.long	4
	.long	100
	.long	68
	.long	4
	.long	4
	.long	102
	.long	70
	.long	6
	.long	6
	.long	102
	.long	70
	.long	6
	.long	6
	.long	103
	.long	71
	.long	7
	.long	7
	.long	103
	.long	71
	.long	7
	.long	7
	.long	104
	.long	72
	.long	8
	.long	8
	.long	104
	.long	72
	.long	8
	.long	8
	.long	106
	.long	74
	.long	10
	.long	10
	.long	106
	.long	74
	.long	10
	.long	10
	.long	107
	.long	75
	.long	11
	.long	11
	.long	107
	.long	75
	.long	11
	.long	11
	.long	108
	.long	76
	.long	12
	.long	12
	.long	108
	.long	76
	.long	12
	.long	12
	.long	59
	.long	58
	.long	0
	.long	0
	.long	59
	.long	58
	.long	0
	.long	0
	.long	39
	.long	34
	.long	0
	.long	0
	.long	39
	.long	34
	.long	0
	.long	0
	.long	96
	.long	126
	.long	0
	.long	0
	.long	96
	.long	126
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	92
	.long	124
	.long	28
	.long	28
	.long	92
	.long	124
	.long	28
	.long	28
	.long	122
	.long	90
	.long	26
	.long	26
	.long	122
	.long	90
	.long	26
	.long	26
	.long	120
	.long	88
	.long	24
	.long	24
	.long	120
	.long	88
	.long	24
	.long	24
	.long	99
	.long	67
	.long	3
	.long	3
	.long	99
	.long	67
	.long	3
	.long	3
	.long	118
	.long	86
	.long	22
	.long	22
	.long	118
	.long	86
	.long	22
	.long	22
	.long	98
	.long	66
	.long	2
	.long	2
	.long	98
	.long	66
	.long	2
	.long	2
	.long	110
	.long	78
	.long	14
	.long	14
	.long	110
	.long	78
	.long	14
	.long	14
	.long	109
	.long	77
	.long	13
	.long	13
	.long	109
	.long	77
	.long	13
	.long	13
	.long	44
	.long	60
	.long	0
	.long	0
	.long	44
	.long	60
	.long	0
	.long	0
	.long	46
	.long	62
	.long	0
	.long	0
	.long	46
	.long	62
	.long	0
	.long	0
	.long	47
	.long	63
	.long	0
	.long	0
	.long	47
	.long	63
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	42
	.long	42
	.long	42
	.long	42
	.long	42
	.long	42
	.long	42
	.long	42
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	32
	.long	32
	.long	0
	.long	32
	.long	32
	.long	32
	.long	0
	.long	32
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	15104
	.long	21504
	.long	24064
	.long	0
	.long	26624
	.long	0
	.long	0
	.long	0
	.long	15360
	.long	21760
	.long	24320
	.long	0
	.long	26880
	.long	0
	.long	0
	.long	0
	.long	15616
	.long	22016
	.long	24576
	.long	0
	.long	27136
	.long	0
	.long	0
	.long	0
	.long	15872
	.long	22272
	.long	24832
	.long	0
	.long	27392
	.long	0
	.long	0
	.long	0
	.long	16128
	.long	22528
	.long	25088
	.long	0
	.long	27648
	.long	0
	.long	0
	.long	0
	.long	16384
	.long	22784
	.long	25344
	.long	0
	.long	27904
	.long	0
	.long	0
	.long	0
	.long	16640
	.long	23040
	.long	25600
	.long	0
	.long	28160
	.long	0
	.long	0
	.long	0
	.long	16896
	.long	23296
	.long	25856
	.long	0
	.long	28416
	.long	0
	.long	0
	.long	0
	.long	17152
	.long	23552
	.long	26112
	.long	0
	.long	28672
	.long	0
	.long	0
	.long	0
	.long	17408
	.long	23808
	.long	26368
	.long	0
	.long	28928
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	18176
	.long	55
	.long	55
	.long	55
	.long	55
	.long	55
	.long	55
	.long	55
	.long	18432
	.long	56
	.long	56
	.long	56
	.long	56
	.long	56
	.long	56
	.long	56
	.long	18688
	.long	57
	.long	57
	.long	57
	.long	57
	.long	57
	.long	57
	.long	57
	.long	45
	.long	45
	.long	45
	.long	45
	.long	45
	.long	45
	.long	45
	.long	45
	.long	19200
	.long	52
	.long	52
	.long	52
	.long	52
	.long	52
	.long	52
	.long	52
	.long	19456
	.long	53
	.long	53
	.long	53
	.long	53
	.long	53
	.long	53
	.long	53
	.long	19712
	.long	54
	.long	54
	.long	54
	.long	54
	.long	54
	.long	54
	.long	54
	.long	43
	.long	43
	.long	43
	.long	43
	.long	43
	.long	43
	.long	43
	.long	43
	.long	20224
	.long	49
	.long	49
	.long	49
	.long	49
	.long	49
	.long	49
	.long	49
	.long	20480
	.long	50
	.long	50
	.long	50
	.long	50
	.long	50
	.long	50
	.long	50
	.long	20736
	.long	51
	.long	51
	.long	51
	.long	51
	.long	51
	.long	51
	.long	51
	.long	20992
	.long	48
	.long	48
	.long	48
	.long	48
	.long	48
	.long	48
	.long	48
	.long	21248
	.long	46
	.long	46
	.long	46
	.long	46
	.long	46
	.long	0
	.long	0
	.local	bufpos
	.comm	bufpos,4,16
	.local	buffer
	.comm	buffer,256,32
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

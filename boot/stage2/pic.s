	.file	"pic.c"
	.text
	.p2align 4,,15
	.globl	pic_init
	.type	pic_init, @function
pic_init:
.LFB0:
	.cfi_startproc
	jmp	i8259_init
	.cfi_endproc
.LFE0:
	.size	pic_init, .-pic_init
	.p2align 4,,15
	.globl	pic_enable
	.type	pic_enable, @function
pic_enable:
.LFB1:
	.cfi_startproc
	jmp	i8259_enable
	.cfi_endproc
.LFE1:
	.size	pic_enable, .-pic_enable
	.p2align 4,,15
	.globl	pic_disable
	.type	pic_disable, @function
pic_disable:
.LFB2:
	.cfi_startproc
	jmp	i8259_disable
	.cfi_endproc
.LFE2:
	.size	pic_disable, .-pic_disable
	.p2align 4,,15
	.globl	pic_eoi
	.type	pic_eoi, @function
pic_eoi:
.LFB3:
	.cfi_startproc
	jmp	i8259_eoi
	.cfi_endproc
.LFE3:
	.size	pic_eoi, .-pic_eoi
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits

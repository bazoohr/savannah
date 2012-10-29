/*
 * we MUST load this code, in a known address
 */
 /* =============================================== */
/* This is fixed address, below the BSP's kernel stack */
#define BASE_ADDR 0x9F000
 /* =============================================== */
.code16
.align 0x10
.section .text
.globl _start
_start:
    cli              /* Make sure interrupts are disabled */
    xorw $(BASE_ADDR >> 4), %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %gs
    movw %ax, %ss

    lgdt realmode_gdt_descr
    movw $0x11, %ax
    lmsw %ax
jmp_32:
  .byte 0x66
  .byte 0xEA
  .long BASE_ADDR + protmode
  .word bootcode - GDT
/* ==================================================== */
/*                     32 bit code                      */
/* ==================================================== */
.code32
.align 0x10
protmode:
  movl $bootdata-GDT, %eax
  movl %eax, %ds
  movl %eax, %es
  movl %eax, %ss
  /* 
   * BSP has already made kernel page tables,
   * so we just load the page table base address
   * into cr3, of application processors (aps)
   */
	movl $0x1000, %eax
  movl %eax, %cr3

  /* Going to enable long-mode */
	movl %cr4, %eax
	btsl $5, %eax
	movl %eax, %cr4

  /*
   * TODO:
   *  Do a cpuid to check whether CPU supports NotExecutable 
   *  pages or not?!
   */
 	movl $0xC0000080, %ecx
	rdmsr
	btsl $8, %eax          /* Long mode enable */
	btsl $11, %eax         /* For Not executable segments */
	wrmsr

  /* Page tables are made and cr3 is set by boot loader.
   * After activating paging, we are in 64-bit mode.
	 * Don't write code between assigning CR0 and jump instruction. */
	movl %cr0, %eax
	btsl $31, %eax
	movl %eax, %cr0
  /* A long jump, to flush pipeline */
	.byte 0xEA
	.long BASE_ADDR + start_64        /* Starting address of 64-bit code */
	.word code64 - GDT    /* 64-bit code selector */
/* ==================================================== */
/*                     64 bit code                      */
/* ==================================================== */
.align 0x10
.code64
start_64:
  movq $(bootdata - GDT), %rax     /* 0x18 is data selector, look at GDT */
  movq %rax, %ds       /* Load data selector into ds, es, fs, gs*/
  movq %rax, %es
  movq %rax, %fs
  movq %rax, %gs
  movb $'A', 0xB8000
  movb $0x4, 0xB8001
  cli;
  hlt
  /* Setup kernel stack */
  movq %rax, %ss
  movq $0xA0000, %rsp
  movq %rsp, %rbp
  /* Switch to kernel's new GDT */

  cli
  movb $'!', 0xb8000
  movb $5,   0xb8001
  movq $0, %rax
  hlt
/* ========================================= */
.align 0x10
GDT:
	.quad 0
code64:
	.quad 0x0020980000000000	
	/*.quad 0x00BF9B000000FFFF*/
bootcode:
	.quad 0x00CF9B000000FFFF
bootdata:
	.quad 0x00CF93000000FFFF
TSS:
	.quad 0
	.quad 0
LDT:
	.quad 0
	.quad 0
GDTEND:
realmode_gdt_descr:
	.word GDTEND - GDT
	.long BASE_ADDR + GDT
protmode_gdt_desc:
longmode_gdt_desc:
	.word GDTEND - GDT
	.long BASE_ADDR + GDT

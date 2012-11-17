/*
 * we MUST load this code in a known address
 */
 /* =============================================== */
 /*
  * TODO:
  *     Probably we will need to move this program to kernel
  *     directory. Because it needs to access asm_cpu_const.h
  *     which is available only in kernel.
  *     Currently I just write this *UGLY* include, to get the job
  *     done, but this must change later.
  */
#include <../kernel/include/asm_cpu_const.h>
#include <../kernel/include/const.h>
 /* =============================================== */
/* This is fixed address, below the BSP's kernel stack */
#define BASE_ADDR 0x9F000
 /* =============================================== */
.code16
.org 0
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
  movl %eax, %ds   /* We only need data segment here */
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
  movq %rax, %ss
  /* =====================================
   * Notmally, before enabling x87 & media instructions, we have to
   * check to see if the processor supports these instructions.
   * But since BSP has already done this check, we simply enable them
   * for all other APs.
   * This must work all the time if the system is symetric.
   * ===================================== */
  /* let the processor know that we are going to use
   * media instructions, by setting cr4.osfxsr(bit number 9)*/
  movq %cr4, %rax
  btsq $9, %rax
  movq %rax, %cr4
  /* No processor emulate coprocessor 
   * by clearing cr0.em (bit number 2) */
  movq %cr0, %rax
  btrq $2, %rax
  movq %rax, %cr0
  /* enable monitor coprocessor by enabling
   * cr0.mp (bit number 1) */
  movq %cr0, %rax
  btsq $1, %rax
  movq %rax, %cr0
  /* Setup kernel stack */
  movq (512 + BASE_ADDR), %rax
  addq $CPU_STACK + KNL_STACK_SIZE, %rax
  movq %rax, %rsp
  movq %rsp, %rbp
  /* Now, APs are ready to jump to kernel */
  movq (512 + BASE_ADDR), %rdi
  movq CPU_CPUID(%rdi), %rdi  /* Argument */
  movq (520 + BASE_ADDR), %rax
  jmp *%rax    /* Jump to kernel */
  /* Switch to kernel's new GDT */

  cli
  movb $'!', 0xb8001
  movb $5,   0xb8002
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
.org 512
/*
 * BSP, will inform APs about their information, and address of 
 * code they have to execute next.
 */
my_info:
  .quad 0   /* Pointer to cpu information */
.org 520
kernel_enterance:
  .quad 0   /* Address of Kernel C code APs must execute */

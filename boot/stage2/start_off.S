/* ========================================= *
 * start_off.S                               *
 *                                           *
 * Hamid R. Bazoobandi                       *
 * Sunday 15 July 2012 Amsterdam             *
 * ========================================= */
#include <page.h>
#include <asm_boot_stage2_args_const.h>
#include <asm_cpu_const.h>
#include <const.h>
#include <config.h>
.code32
.section .text
.align 0x10
.globl _start
_start:
  cli              /* Make sure interrupts are disabled */
	lgdt gdt_descr   /* Reload GDT */
	.byte 0xEA       /* Long jumpi. Flush pipeline */
	.long cont       /* Address to continue after long jump */
	.word bootcode - GDT       /* Code selector */
cont:

  /* Zero out the page tables */
  cld
  movl $(0x4000 - 0x1000), %ecx
  movl $0x1000, %edi
  movl $0, %eax
  rep stosb

	movl $0x3000, %edi
	movl $0, %ecx
1:
  movl $BSP_PAGE_SIZE, %eax
	mull %ecx
	addl $(PAGE_PRESENT | PAGE_RW | PAGE_PSE), %eax
	movl %eax, (%edi)
	movl $0, 4(%edi)
	addl $8, %edi
	incl %ecx
	cmpl $PAGE_TABLE_ENTRIES, %ecx
	jne 1b

	movl $0x3000 | PAGE_PRESENT, 0x2000
	movl $0x0000, 0x2000 + 4

	movl $0x2000 | PAGE_PRESENT, 0x1000
	movl $0x0000, 0x1000 + 4

	movl $0x1000, %eax
	movl %eax, %cr3

  movl 4(%esp), %ebx
  movl $0x1000, BOOT_STAGE2_PGTB(%ebx)

  movl 4(%esp), %edi  /* Save kernel arguments. */
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
	.long start_64        /* Starting address of 64-bit code */
	.word code64 - GDT    /* 64-bit code selector */
/* ==================================================== */
/*                     64 bit code                      */
/* ==================================================== */
.align 0x10
.code64
start_64:
  /*
   * XXX:
   *     RDI contains kernel arguments. Don't change its contents.
   */
  movq $(bootdata - GDT), %rax     /* 0x18 is data selector, look at GDT */
  movq %rax, %ds       /* Load data selector into ds, es, fs, gs*/
  movq %rax, %es
  movq %rax, %fs
  movq %rax, %gs
  movq %rax, %ss
  /* =====================================
   * FUCK Commented Line Below! Took me one week to
   * find the bug caused by it.
   *
   * With kind regards,
   * Hamid
   *
   * movq $cpus + CPU_STACK + KNL_STACK_SIZE, %rsp
   */
  /* Setup BSP's kernel stack (BSP's cpuid = 0) */
  movq $0xA0000, %rsp
  movq %rsp, %rbp
  /* Switch to kernel's new GDT */
  pushq %rdi      /* Save rdi (kernel Arguments) */
  callq create_new_gdt
  popq %rdi       /* Reload rdi (kernel Arguments) */
  /* Now we call main () of kernel */
  movq $0, %rax
  movl %edi, %eax
  movq %rax, %rdi
  callq boot_stage2_main

  cli
  movb $'!', 0xb8000
  movb $5,   0xb8001
  movq $0, %rax
  hlt
/* ========================================= */
.section .data
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
gdt_descr:
	.word GDTEND - GDT
	.long GDT

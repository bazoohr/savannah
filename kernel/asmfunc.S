/* ======================================== *
 * asmfunc.S                                *
 *                                          *
 * Hamid R. Bazoobandi 2012                 *
 * Amsterdam, The Netherlands               *
 * ======================================== */
.code64
.section .text
#include <proc.h>
#include <const.h>
#include <dev/pic.h>
#include <asmlink.h>
#include <lock.h>
/* ============================ */
/* void cpuid (register_t *rax, register_t *rbx
          register_T *rcx, register_t *rdx);
*/
ENTRY(cpuid)
  pushq %rbx
  pushq %rdx     /* Third argument */
  pushq %rcx     /* Fourth argument */

  movq (%rdi), %rax   /* Function number */
  cpuid

  movq %rax, (%rdi)
  /* If rsi == null; then skip */
  cmpq $0, %rsi
  je 1f
  movq %rbx, (%rsi)
1:
  popq %rsi
  /* If rsi == null; then skip */
  cmpq $0, %rsi
  je 1f
  movq %rdx, (%rsi)
1:
  popq %rsi
  /* If rsi == null; then skip */
  cmpq $0, %rsi
  je 1f
  movq %rcx, (%rsi)
1:
  popq %rbx
  retq
END(cpuid)
#if 0
/* ============================ */
ENTRY(rdmsr)
  movq $0, %rax
  movq $0, %rdx
  /* MSR address is 32bit */
  movl %edi, %ecx 
  /* Result would be edx:eax */
  rdmsr
  /* Shift left rdx 32 bit   */
  movb $32, %cl
  shlq %cl, %rdx
  /* Aggregate result of rdmsr into rax */
  addq %rdx, %rax
  retq
END(rdmsr)
/* ============================ */
ENTRY(wrmsr)
  
END(wrmsr)
#endif
/* ============================ */
ENTRY (cli)
  cli
  retq
END (cli)
/* ============================ */
ENTRY (inb)
	movw %di, %dx
	inb %dx
	retq
END (inb)
/* ============================ */
ENTRY (outb)
	movw %di, %ax
	movw %si, %dx
	outb %al, %dx
	retq
END(outb)
/* ============================ */
ENTRY (reload_gdt)
  cli
  lgdt (%rdi)

  movl %edx, %ds
  movl %edx, %es
  movl %edx, %fs
  movl %edx, %gs
  movl %edx, %ss

  pushq %rsi
  pushq $1f
  lretq
1:
  retq
END (reload_gdt)
/* ================================== */
ENTRY (lidt)
  lidt (%rdi)
  retq
END (lidt)
/* ================================== */
ENTRY (load_cr3)
  movq %rdi, %rax
  movq %rax, %cr3
  retq
END (load_cr3)
/* ================================== */
ENTRY (read_cr2)
  movq %cr2, %rax
  retq
END (read_cr2)
/* ======================================= */
ENTRY (switchto)
#if 0
  movq %rax, PR_RAX(%rdi)
  movq %rbx, PR_RBX(%rdi)
  movq %rcx, PR_RCX(%rdi)
  movq %rdx, PR_RDX(%rdi)
  movq %r8,  PR_R8(%rdi)
  movq %r9,  PR_R9(%rdi)
  movq %r10, PR_R10(%rdi)
  movq %r11, PR_R11(%rdi)
  movq %r12, PR_R12(%rdi)
  movq %r13, PR_R13(%rdi)
  movq %r14, PR_R14(%rdi)
  movq %r15, PR_R15(%rdi)

  movq %rbp, PR_RBP(%rdi)
  movq %rsp, PR_RSP(%rdi)
  movq %rdi, PR_RDI(%rdi)
  movq %rsi, PR_RSI(%rsi)
  movq $resume, PR_RIP(%rdi)

  pushfq
  popq %rax
  movq %rax, PR_RFLAGS (%rdi)

  movq %cs, %rax
  movq %rax, PR_CS(%rdi)
  movq %ds, %rax
  movq %rax, PR_DS(%rdi)
  movq %es, %rax
  movq %rax, PR_ES(%rdi)
  movq %fs, %rax
  movq %rax, PR_FS(%rdi)
  movq %gs, %rax
  movq %rax, PR_GS(%rdi)
  movq %ss, %rax
  movq %rax, PR_SS(%rdi)

  movq %cr3, %rax
  movq %rax, PR_CR3(%rdi)

  /*
   * Currently we are using the old process stack and page tables. Before going any further
   * we have to first switch to next process's page tables and then change the stack to the one belongs to
   * next process.
   */
  
  pushq %rsi
  leaq PR_LDT_DESC(%rsi), %rdi
  callq gdt_update_ldt
  popq %rsi

  pushq %rsi
  leaq PR_TSS_DESC(%rsi), %rdi
  callq gdt_update_tss
  popq %rsi

  movq %rsi, current     /* Set current process */

  movq PR_MAX_QUANTUM(%rsi), %rax
  movq %rax, PR_REMAINED_QUANTUM(%rsi)

  movq PR_CR3(%rsi), %rax
  movq %rax, %cr3

  /* Next process stack */
  movq PR_RSP(%rsi), %rsp
  /* Restore registers from PCB */
  movq PR_RBX(%rsi), %rbx
  movq PR_RCX(%rsi), %rcx
  movq PR_RDX(%rsi), %rdx
  movq PR_R8(%rsi), %r8
  movq PR_R9(%rsi), %r9
  movq PR_R10(%rsi), %r10
  movq PR_R11(%rsi), %r11
  movq PR_R12(%rsi), %r12
  movq PR_R13(%rsi), %r13
  movq PR_R14(%rsi), %r14
  movq PR_R15(%rsi), %r15

  movq PR_RBP(%rsi), %rbp
  movq PR_RDI(%rsi), %rdi

  movq $LDT_SEL, %rax
  lldt %ax

  movq $TSS_SEL, %rax
  ltr %ax

  movq PR_DS(%rsi), %rax
  movq %rax, %ds
  movq PR_ES(%rsi), %rax
  movq %rax, %es
  movq PR_FS(%rsi), %rax
  movq %rax, %fs
  movq PR_GS(%rsi), %rax
  movq %rax, %gs

  movq PR_RAX(%rsi), %rax
  /* prepare everything for the next iretq. */
  pushq PR_SS(%rsi)
  pushq PR_RSP(%rsi)
  pushq PR_RFLAGS(%rsi)
  pushq PR_CS(%rsi)
  pushq PR_RIP(%rsi)

  movq PR_RSI(%rsi), %rsi

  iretq
resume:
  retq
#endif
END (switchto)
/* =========================================== */
ENTRY (idle)
#if 0
  cli

  movq $IDLE_PID, %rdi
  callq get_pcb

  movq %rax, %rsi

  leaq PR_LDT_DESC(%rsi), %rdi
  callq gdt_update_ldt
  
  leaq PR_TSS_DESC(%rsi), %rdi
  callq gdt_update_tss

  /* Set current process to idle (This is the first process)*/
  movq %rsi, current     
  
  movq $LDT_SEL, %rax
  lldt %ax

  movq $TSS_SEL, %rax
  ltr %ax

  pushq $SYS_DSEL
  pushq %rsp
  pushfq
  pushq $SYS_CSEL
  pushq $1f
  iretq
1:
  movq $SYS_DSEL, %rax
  movl %eax, %ds
  movl %eax, %ss
  movl %eax, %fs
  movl %eax, %gs
2:
  sti
  hlt
  cli
  jmp 2b
  
  /* We MUST never reach to this point */
  retq 
#endif
END (idle)
/* =========================================== */
ENTRY (fast_memcpy)
  movq %rdi, %r8
  movq %rdx, %rcx
  rep movsb
  movq %r8, %rax
  retq
END (fast_memcpy)
/* =========================================== */
ENTRY (fast_memset)
  /* Should we clear the direction flag here?*/
  cld
  movq %rdi, %r8
  movq %rdx, %rcx
  movq %rsi, %rax
  rep stosb
  movq %r8, %rax
  retq
END (fast_memset)
/* =========================================== */
/*
 * Lock a critical region by disabling interrupts
 */
ENTRY (lock_region)
  pushfq
  popq %rcx

  btq $9, %rcx
  jnc no_int

  movl $REGION_LOCKED, (%rdi)
  cli
  jmp 1f
/*
 * If region is already locked
 */
no_int: 
  movl $REGION_NOT_LOCKED, (%rdi)
1:
  retq
END (lock_region)
/* =========================================== */
ENTRY (unlock_region)
  cmpl $REGION_LOCKED, (%rdi)
  jne 1f
  sti
1:
  retq
END (unlock_region)

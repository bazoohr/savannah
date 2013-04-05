#ifndef __TRAP_H__
#define __TRAP_H__

#include <frame.h>
#include <const.h>

#define RSVD_TRAP   (NIDT + 1)   /* reserved trap number */
/*
 * Entries in the Interrupt Descriptor Table (IDT)
 */
#define	IDT_DE		0	/* #DE: Divide Error */
#define	IDT_DB		1	/* #DB: Debug */
#define	IDT_NMI		2	/* Nonmaskable External Interrupt */
#define	IDT_BP		3	/* #BP: Breakpoint */
#define	IDT_OF		4	/* #OF: Overflow */
#define	IDT_BR		5	/* #BR: Bound Range Exceeded */
#define	IDT_UD		6	/* #UD: Undefined/Invalid Opcode */
#define	IDT_NM		7	/* #NM: No Math Coprocessor */
#define	IDT_DF		8	/* #DF: Double Fault */
#define	IDT_FPUGP	9	/* Coprocessor Segment Overrun */
#define	IDT_TS		10	/* #TS: Invalid TSS */
#define	IDT_NP		11	/* #NP: Segment Not Present */
#define	IDT_SS		12	/* #SS: Stack Segment Fault */
#define	IDT_GP		13	/* #GP: General Protection Fault */
#define	IDT_PF		14	/* #PF: Page Fault */
#define	IDT_MF		16	/* #MF: FPU Floating-Point Error */
#define	IDT_AC		17	/* #AC: Alignment Check */
#define	IDT_MC		18	/* #MC: Machine Check */
#define	IDT_XF		19	/* #XF: SIMD Floating-Point Exception */

#define IDT_SYSCALL TRAP_SYSCALL  /* IDT vector for system calls */

#define IST   0       /* We're not going to use IST */

#ifndef __ASSEMBLY__
void reserved (void);
void divide_error (void);
void debug (void);
void breakpoint (void);
void nmi (void);
void overflow (void);
void bound_range (void);
void invalid_opcode (void);
void device_not_available (void);
void double_fault (void);
void invalid_tss (void);
void segment_not_present (void);
void stack_exception (void);
void general_protection (void);
void page_fault (void);
void fp_exception (void);
void alignment_check (void);
void machine_check (void);
void xmm (void);
void system_call (void);
void trap_handler (struct intr_stack_frame *tf);
#endif

#endif

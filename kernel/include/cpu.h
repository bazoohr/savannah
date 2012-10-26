#ifndef __CPU_H__
#define __CPU_H__

#include <types.h>
#include <cdef.h>

struct regs {
	register_t rax;
	register_t rbx;
	register_t rcx;
	register_t rdx;
	register_t rsi;
	register_t rdi;
	register_t rsp;
	register_t rbp;
	register_t rip;
	register_t cs;
	register_t ds;
	register_t es;
	register_t fs;
	register_t gs;
	register_t ss;
	register_t rflags;
	register_t r8;
	register_t r9;
	register_t r10;
	register_t r11;
	register_t r12;
	register_t r13;
	register_t r14;
	register_t r15;
	register_t cr3;
}__packed;

// Control Register flags
#define CR0_PE		0x00000001	// Protection Enable
#define CR0_MP		0x00000002	// Monitor coProcessor
#define CR0_EM		0x00000004	// Emulation
#define CR0_TS		0x00000008	// Task Switched
#define CR0_ET		0x00000010	// Extension Type
#define CR0_NE		0x00000020	// Numeric Error
#define CR0_WP		0x00010000	// Write Protect
#define CR0_AM		0x00040000	// Alignment Mask
#define CR0_NW		0x20000000	// Not Writethrough - more tricky than it sounds
#define CR0_CD		0x40000000	// Cache Disable
#define CR0_PG		0x80000000	// Paging

// These two relate to the cacheability (L1, etc) of the page directory
#define CR3_PWT		0x00000008	// Page directory caching write through
#define CR3_PCD		0x00000010	// Page directory caching disabled

#define CR4_VME		0x00000001	// V86 Mode Extensions
#define CR4_PVI		0x00000002	// Protected-Mode Virtual Interrupts
#define CR4_TSD		0x00000004	// Time Stamp Disable
#define CR4_DE		0x00000008	// Debugging Extensions
#define CR4_PSE		0x00000010	// Page Size Extensions
#define CR4_PAE		0x00000020	// Physical Address Extensions
#define CR4_MCE		0x00000040	// Machine Check Enable
#define CR4_PGE		0x00000080	// Global Pages Enabled
#define CR4_PCE		0x00000100	// Performance counter enable
#define CR4_OSFXSR	0x00000200	// OS support for FXSAVE/FXRSTOR
#define CR4_OSXMME	0x00000400	// OS support for unmasked SIMD FP exceptions
#define CR4_VMXE	0x00002000	// VMX enable
#define CR4_SMXE	0x00004000	// SMX enable
#define CR4_OSXSAVE	0x00040000	// XSAVE and processor extended states-enabled

#define MTRR_DEF_TYPE			  0x2ff
#define MTRR_PHYSBASE0			0x200
#define MTRR_PHYSMASK0			0x201
#define MTRR_PHYSBASE1			0x202
#define MTRR_PHYSMASK1			0x203
#define MTRR_PHYSBASE2			0x204
#define MTRR_PHYSMASK2			0x205
#define MTRR_PHYSBASE3			0x206
#define MTRR_PHYSMASK3			0x207
#define MTRR_PHYSBASE4			0x208
#define MTRR_PHYSMASK4			0x209
#define MTRR_PHYSBASE5			0x20a
#define MTRR_PHYSMASK5			0x20b
#define MTRR_PHYSBASE6			0x20c
#define MTRR_PHYSMASK6			0x20d
#define MTRR_PHYSBASE7			0x20e
#define MTRR_PHYSMASK7			0x20f

#endif


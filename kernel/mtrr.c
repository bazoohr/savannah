#include <types.h>
#include <printk.h>
#include <cpu.h>
#include <asmfunc.h>

#define MTRR_UC 0x00   /* Uncacheable */
#define MTRR_WC 0x01   /* Write Combining */
#define MTRR_WT 0x04   /* Write Through */
#define MTRR_WP 0x05   /* Write Protect */
#define MTRR_WB 0x06   /* Write Back */

// could consider having an API to allow these to dynamically change
// MTRRs are for physical, static ranges.  PAT are linear, more granular, and 
// more dynamic
void setup_default_mtrrs(void)
{
	// disable interrupts
  cli ();
	// disable caching	cr0: set CD and clear NW
	lcr0((rcr0() | CR0_CD) & ~CR0_NW);
	// flush caches
	cache_flush();
	// flush tlb
	tlb_flush_global();
	// disable MTRRs, and sets default type to WB (06)
	wrmsr(MTRR_DEF_TYPE, MTRR_WB);

#if 0
	// Now we can actually safely adjust the MTRRs
	// MTRR for IO Holes (note these are 64 bit values we are writing)
	// 0x000a0000 - 0x000c0000 : VGA - WC 0x01
	wrmsr(MTRR_PHYSBASE0, PTE_ADDR(VGAPHYSMEM) | 0x01);
	// if we need to have a full 64bit val, use the UINT64 macro
	wrmsr(MTRR_PHYSMASK0, 0x0000000ffffe0800);
	// 0x000c0000 - 0x00100000 : IO devices (and ROM BIOS) - UC 0x00
	wrmsr(MTRR_PHYSBASE1, PTE_ADDR(DEVPHYSMEM) | 0x00);
	wrmsr(MTRR_PHYSMASK1, 0x0000000ffffc0800);
#endif
	wrmsr(MTRR_PHYSBASE0, 0xFEA00000 | MTRR_UC);
	wrmsr(MTRR_PHYSMASK0, 0xFFFFFFEA00800);
	// make sure all other MTRR ranges are disabled (should be unnecessary)
	wrmsr(MTRR_PHYSMASK1, 0);
	wrmsr(MTRR_PHYSMASK2, 0);
	wrmsr(MTRR_PHYSMASK3, 0);
	wrmsr(MTRR_PHYSMASK4, 0);
	wrmsr(MTRR_PHYSMASK5, 0);
	wrmsr(MTRR_PHYSMASK6, 0);
	wrmsr(MTRR_PHYSMASK7, 0);

	// keeps default type to WB (06), turns MTRRs on, and turns off fixed ranges
	wrmsr(MTRR_DEF_TYPE, 0x00000806);
	// reflush caches and TLB
	cache_flush();
	tlb_flush_global();
	// turn on caching
	lcr0(rcr0() & ~(CR0_CD | CR0_NW));
}

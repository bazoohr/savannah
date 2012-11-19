#ifndef __SEGMENT_H__
#define __SEGMENT_H__

#include <types.h>
#include <cdef.h>

struct system_descriptor {
	uint64_t sd_lolimit:16;
	uint64_t sd_lobase:24;
	uint64_t sd_type:5;
#define SD_64LDT        0x2
#define SD_64TSS        0x9
	uint64_t sd_dpl:2;
	uint64_t sd_p:1;
	uint64_t sd_hilimit:4;
	uint64_t sd_avl:1;
	uint64_t sd_resvd1:2;
	uint64_t sd_G:1;
	uint64_t sd_hibase:40;
	uint64_t sd_resvd2:8;
	uint64_t sd_zero:5;
	uint64_t sd_resvd3:19;
} __packed;

struct code64_descriptor {
  uint64_t cd_ignore1:42;
  uint64_t cd_c:1;
  uint64_t cd_three:2;
  uint64_t cd_dpl:2;
  uint64_t cd_p:1;
  uint64_t cd_ignore2:5;
  uint64_t cd_long:1;
  uint64_t cd_opndsz:1;
  uint64_t cd_ignored3:9;
} __packed;

struct data64_descriptor {
  uint64_t dd_ignore1:41;
  uint64_t dd_w:1;
  uint64_t dd_ignore2:2;
  uint64_t dd_one:1;
  uint64_t dd_dpl:2;
  uint64_t dd_p:1;
  uint64_t dd_ignore3:16;
} __packed;

struct ldt {
  struct code64_descriptor code;
  struct data64_descriptor data;
} __packed;

struct gate_descriptor {
	uint64_t gd_looffset:16;	/* gate offset (lsb) */
	uint64_t gd_sel:16;	/* gate segment selector */
	uint64_t gd_ist:3;		/* IST table index */
	uint64_t gd_unusd1:5;		/* unused */
#define IDT_TRAP_GATE  0xF   /* Trap Gate Descriptor Type */
#define IDT_INTR_GATE  0xE   /* Interrupt Gate Descriptor Type */
	uint64_t gd_type:5;		/* segment type */
	uint64_t gd_dpl:2;		/* segment descriptor priority level */
	uint64_t gd_p:1;		/* segment descriptor present */
	uint64_t gd_hioffset:48 __packed;	/* gate offset (msb) */
	uint64_t gd_unusd2:32;
} __packed;

/*
 * region descriptors, used to load gdt/idt tables before segments yet exist.
 */
struct descriptor_register {
	uint16_t dr_limit;		/* segment extent */
	uint64_t dr_base;   	/* base address  */
} __packed;

struct tss {
	uint32_t	tss_rsvd0;
	uint64_t	tss_rsp0 __packed; 	/* kernel stack pointer ring 0 */
	uint64_t	tss_rsp1 __packed; 	/* kernel stack pointer ring 1 */
	uint64_t	tss_rsp2 __packed; 	/* kernel stack pointer ring 2 */
	uint32_t	tss_rsvd1;
	uint32_t	tss_rsvd2;
	uint64_t	tss_ist1 __packed;	/* Interrupt stack table 1 */
	uint64_t	tss_ist2 __packed;	/* Interrupt stack table 2 */
	uint64_t	tss_ist3 __packed;	/* Interrupt stack table 3 */
	uint64_t	tss_ist4 __packed;	/* Interrupt stack table 4 */
	uint64_t	tss_ist5 __packed;	/* Interrupt stack table 5 */
	uint64_t	tss_ist6 __packed;	/* Interrupt stack table 6 */
	uint64_t	tss_ist7 __packed;	/* Interrupt stack table 7 */
	uint32_t	tss_rsvd3;
	uint32_t	tss_rsvd4;
	uint16_t	tss_rsvd5;
	uint16_t	tss_iobase;	/* io bitmap offset */
} __packed;

void __inline gdt_update_tss (struct system_descriptor *tss_desc);
void __inline gdt_update_ldt (struct system_descriptor *ldt_desc);
void create_ldt_descriptor (struct ldt *ldt_addr, struct system_descriptor *ldt_desc);
void create_tss_descriptor (struct tss *tss_addr, struct system_descriptor *tss_desc);
void create_ldt (struct ldt *ldt, int dpl);
void create_new_gdt (cpuid_t cpuid);
#endif

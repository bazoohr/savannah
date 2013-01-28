#ifndef __CPU_H__
#define __CPU_H__

#ifndef __ASSEMBLY__ 

#include <types.h>
#include <cdef.h>
#include <const.h>

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

struct cpu_info {
  uint8_t lapic_id; // Local APIC ID
  uint8_t cpuid;  // Kernel CPU ID
  volatile uint8_t booted;  // Has the CPU completed booting?
  volatile uint8_t ready;   // Is the CPU ready to launch the VM?
  virt_addr_t vmm_vstack;       // CPU stack (Will be VMM's stack)
  phys_addr_t vmm_page_tables;  // CPU page tables (VMM page tables
  phys_addr_t vmm_ept_tables;   // CPU EPT page tables
  phys_addr_t vmm_start_paddr; // VMM's physical start address
  phys_addr_t vmm_end_paddr;   // VMM's physical end address
  virt_addr_t vmm_start_vaddr; // VMM's virtual start address
  virt_addr_t vmm_end_vaddr;   // VMM's virtual end address

  virt_addr_t vm_vstack;       // CPU stack (Will be VMM's stack)
  phys_addr_t vm_start_paddr; // VM's physical start address
  phys_addr_t vm_page_tables;  // CPU page tables (VM page tables
  phys_addr_t vm_end_paddr;   // VM's physical end address
  virt_addr_t vm_start_vaddr; // VM's virtual start address
  virt_addr_t vm_end_vaddr;   // VM's virtual end address

  phys_addr_t vm_vmcs_ptr;   // VMX VMC Pointer (must be 4KB aligned)
  phys_addr_t vm_vmxon_ptr;  // VMXON pointer   (must be 4KB aligned)

  struct system_descriptor gdt[NGDT] __aligned (16);
};
struct cpu_info * cpu_alloc (void);
struct cpu_info * get_cpu_info (cpuid_t cpuid);
uint64_t get_ncpus (void);

#endif
/* =============================================== 
 * These macros are used for two purposes
 * 1. Booting application processors
 * 2. Sending information in CPU structure to all
 *    VMMs and VMs.
 *    Each cpu has one VMM and one VM. VMM and VM
 *    are to distinct programs, therefore they can
 *    not send arguments to eachother. The only way
 *    they can send information to eachother in
 *    boot time is to use shared memory. Therefore
 *    they use a know address to share the CPU structre
 *    pointer.
 */
#define BOOT_APS_BASE_ADDR 0x9F000
#define BOOT_APS_LENGTH    512

#if BOOT_APS_BASE_ADDR + BOOT_APS_LENGTH > 0xA0000
#error boot_aps program overlaps with read-only area in memory
#endif

#define CPU_INFO_PTR_ADDR  (BOOT_APS_BASE_ADDR + BOOT_APS_LENGTH)
/* ============================================= */
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


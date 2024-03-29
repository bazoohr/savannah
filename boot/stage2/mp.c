#include <string.h>
#include <asmfunc.h>
#include <types.h>
#include <panic.h>
#include <dev/ioapic.h>
#include <dev/lapic.h>
#include <mp.h>
#include <printk.h>
#include <cpu.h>
#include <memory.h>
#include <cpuinfo.h>
/* ============================================= */
#define IOAPICPA   0xFEC00000	// Default physical address of IO APIC
/* ============================================= */
#define MP_PROC    0x00         // One per processor
#define MP_BUS     0x01         // One per bus
#define MP_IOAPIC  0x02         // One per I/O APIC
#define MP_IOINTR  0x03         // One per bus interrupt source
#define MP_LINTR   0x04         // One per system interrupt source
/* ============================================= */
struct cpu_info * add_cpu ();
/* ============================================= */
struct mp_fptr {
	uint8_t signature[4];       // "_MP_"
	uint32_t physaddr;          // phys addr of MP config table
	uint8_t length;             // 1
	uint8_t specrev;            // [14]
	uint8_t checksum;           // all bytes must add up to 0
	uint8_t type;               // MP system config type
	uint8_t imcrp;
	uint8_t reserved[3];
};

struct mp_conf {
	uint8_t signature[4];       // "PCMP"
	uint16_t length;            // total table length
	uint8_t version;            // [14]
	uint8_t checksum;           // all bytes must add up to 0
	uint8_t product[20];        // product id
	uint32_t oemtable;          // OEM table pointer
	uint16_t oemlength;         // OEM table length
	uint16_t entry;             // entry count
	uint32_t lapicaddr;         // address of local APIC
	uint16_t xlength;           // extended table length
	uint8_t xchecksum;          // extended table checksum
	uint8_t reserved;
};
struct mp_proc {
    uint8_t type;		// entry type (0)
    uint8_t apicid;		// local APIC id
    uint8_t version;		// local APIC verison
    uint8_t flags;		// CPU flags
    uint8_t signature[4];	// CPU signature
    uint32_t feature;		// feature flags from CPUID instruction
    uint8_t reserved[8];
};
struct mp_ioapic {
    uint8_t type;		// entry type (2)
    uint8_t apicno;		// I/O APIC id
    uint8_t version;		// I/O APIC version
    uint8_t flags;		// I/O APIC flags
    uint32_t addr;		// I/O APIC address
};
struct mp_bus {
    uint8_t type;		// entry type(1)
    uint8_t busid;		// bus id
    char busstr[6];		// string which identifies the type of this bus
};
/* ============================================= */
static uint8_t
sum (uint8_t *a, uint32_t length)
{
	uint8_t s = 0;
	uint32_t i;
	for (i = 0 ; i < length ; i++) {
		s += a[i];
	}
	return s;
}
/* ============================================= */
static struct mp_fptr *
mp_search (phys_addr_t pa, int len)
{
	uint8_t *start = (uint8_t *)(pa);
	uint8_t *p;

	for (p = start ; p < (start + len) ; p += sizeof(struct mp_fptr)) {
		if ((memcmp(p, "_MP_", 4) == 0)
		    && (sum(p, sizeof(struct mp_fptr)) == 0))
			return (struct mp_fptr *) p;
	}

	return 0;
}
/* ============================================= */
void
mp_bootothers (void)
{
  cpuid_t i;
  struct cpu_info *cpu;

  cli ();

  for (i = 1; i < get_ncpus (); i++) {
    cpu = get_cpu_info (i);

    lapic_startaps (cpu->lapic_id);

    while (!cpu->ready)
      /* Wait*/;
  }
  /*
   * XXX:
   *     Here, the BSP switchs to become process manager PM.
   *     To this end, we have to switch to page tables of VMM considered for PM.
   *     Then, we have to change the stack to the virtual VMM stack address. This
   *     is what happening in below two lines of assembly.
   */
  __asm__ __volatile__ ("movq %0, %%cr3\n\t"
                        "movq %1, %%rsp\n\t"
                        "movq %%rsp, %%rbp\n\t"
                        "movq %2, (%3)\n\t"
                        "movq %2, %%rdi\n\t"
                        "movq %4, %%rax\n\t"
                        "jmp *%%rax\n\t"::
                        "r"(get_cpu_info (PM)->vmm_info.vmm_page_tables),
                        "r"(get_cpu_info(PM)->vmm_info.vmm_regs.rsp),
                        "r"(get_cpu_info (PM)),
                        "r"(CPU_INFO_PTR_ADDR),
                        "r"(get_cpu_info (PM)->vmm_info.vmm_start_vaddr)
                        );
  /* We MUST NOT get to this point */
  panic ("Failed to run VMM on BootStrap Processor");
}
/* ============================================= */
static int
intel_get_core_number(void)
{
	uint32_t eax;

	cpuid_count (0x4, 0, &eax, NULL, NULL, NULL);

	//printk ("Real Logical IDs: %d\n", (eax >> 26) + 1);

	return (eax >> 26) + 1;
}
/* ============================================= */
void
mp_init(void)
{
	struct mp_fptr *mp;
	uint32_t eax;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;

	cpuid (0x1, &eax, &ebx, &ecx, &edx);
	if (! ((edx & (1 << 9)) && (rdmsr (0x1B) & (1<<11))))
		panic("APIC is NOT supported!");

	/* This code is printing the maximum number of addressable ID for
	 * logical processors in a phyisical package.
	 */
	/*
	cpuid (0x1, NULL, &ebx, NULL, NULL);
	printk ("Logical IDs: %d\n", (ebx & 0x7F8000) >> 16);
	*/

	mp = mp_search(0xF0000, 0x10000);
	if (!mp) {
		panic("No MP floating pointer found");
	}

	if (mp->physaddr == 0 || mp->type != 0) {
		panic("Legacy MP configurations not supported");
	}

	if (mp->imcrp & 0x80)
		panic("PIC Mode not supported\n");

	/*
	 * Because physaddr varibale is 32 bit and in 64-bit mode
	 * all pointers are 64 bit, GCC complains about the below assignment
	 * therefore to cheat it, we cast mp->physaddr to phys_addr_t.
	 */
	struct mp_conf *conf = (struct mp_conf *)((phys_addr_t)mp->physaddr);
	if ((memcmp(conf->signature, "PCMP", 4) != 0)
			|| (sum((uint8_t *) conf, conf->length) != 0)
			|| (conf->version != 1 && conf->version != 4)) {
		cprintk ("Bad Configuration Table %d %d %d\n", 0x4, memcmp(conf->signature, "PCMP", 4) != 0, sum((uint8_t *) conf, conf->length) != 0, conf->version != 1 && conf->version != 4);
		__asm__ __volatile__ ("cli;hlt\n\t");
	}

	struct mp_ioapic *ioapic_conf = NULL;
	uint8_t *start = (uint8_t *)(conf + 1);
	uint8_t *p;
	uint64_t curr_cpu= 0;
	struct cpu_info *curr_cpu_info;
	int number_cores = 0;
	int i;


	for (p = start ; p < ((uint8_t *) conf + conf->length) ; ) {
		switch(*p) {
			case MP_PROC:
				number_cores = intel_get_core_number();

				for (i = 0 ; i < number_cores ; i++) {
					curr_cpu_info = add_cpu ();

					curr_cpu_info->cpuid = curr_cpu;
					curr_cpu_info->lapic_id = curr_cpu;

					curr_cpu++;
				}

				p += sizeof (struct mp_proc);
				continue;
			case MP_IOAPIC:
				/*
				 * IOAPIC address MUST always be 0xFEC00000 not anything else.
				 * This is according to Intel hardware spec.
				 */
				ioapic_conf = (struct mp_ioapic *) p;
				if (ioapic_conf->addr != 0xFEC00000) {
					panic ("IOAPIC address is not valid");
				}
				p += 8;
				continue;
			case MP_IOINTR:
				p += 8;
				continue;
			case MP_LINTR:
				p += 8;
				continue;
			case MP_BUS:
				p += 8;
				continue;
			default:
				panic("Unknown config type");
		}
	}

	if (curr_cpu != get_ncpus ()) {
		panic ("mp failed to properly detect all cpus!");
	}
	if (curr_cpu < MIN_CPUS) {
		//    cprintk ("%d CPUs found! That's too little for this operating system!\n", 0x4, ncpus);
		panic ("%d CPUs found. The minimum number is %d", curr_cpu, MIN_CPUS);
	}

	phys_addr_t ioapicpa;
	if (ioapic_conf) {
		ioapicpa = ioapic_conf->addr;
	} else {
		ioapicpa = IOAPICPA;
	}

	ioapicva = (uint8_t*)ioapicpa;
}

#include <trap.h>
#include <asmfunc.h>

static struct gate_descriptor idt[NIDT] __aligned (16);
static struct descriptor_register idtr;

static void
setgate (int idx, phys_addr_t handler, int typ, int dpl, int ist)
{
	struct gate_descriptor *gd = &idt[idx];

	gd->gd_looffset = handler;
  /* XXX: Should we use a different selector for system calls??? */
	gd->gd_sel = KNL_CSEL;
	gd->gd_ist = ist;
	gd->gd_unusd1 = 0;
	gd->gd_type = typ;
	gd->gd_dpl = dpl;
	gd->gd_p = 1;
	gd->gd_hioffset = handler >> 16;
  gd->gd_unusd2 = 0;
}

static void
trap_init (void)
{
	setgate (IDT_DE, (phys_addr_t)&divide_error,  IDT_TRAP_GATE, KNL_PVL, IST);
	setgate (IDT_DB, (phys_addr_t)&debug,  IDT_TRAP_GATE, KNL_PVL, IST);
	setgate (IDT_NMI, (phys_addr_t)&nmi,  IDT_TRAP_GATE, KNL_PVL, IST);
  setgate (IDT_BP, (phys_addr_t)&breakpoint,  IDT_TRAP_GATE, KNL_PVL, IST);
	setgate (IDT_OF, (phys_addr_t)&overflow,  IDT_TRAP_GATE, KNL_PVL, IST);
	setgate (IDT_BR, (phys_addr_t)&bound_range,  IDT_TRAP_GATE, KNL_PVL, IST);
	setgate (IDT_UD, (phys_addr_t)&invalid_opcode,  IDT_TRAP_GATE, KNL_PVL, IST);
	setgate (IDT_NM, (phys_addr_t)&device_not_available,  IDT_TRAP_GATE, KNL_PVL, IST);
	setgate (IDT_DF, (phys_addr_t)&double_fault, IDT_TRAP_GATE, KNL_PVL, IST);
	setgate (IDT_FPUGP, (phys_addr_t)&reserved,  IDT_TRAP_GATE, KNL_PVL, IST);
	setgate (IDT_TS, (phys_addr_t)&invalid_tss,  IDT_TRAP_GATE, KNL_PVL, IST);
	setgate (IDT_NP, (phys_addr_t)&segment_not_present,  IDT_TRAP_GATE, KNL_PVL, IST);
	setgate (IDT_SS, (phys_addr_t)&stack_exception,  IDT_TRAP_GATE, KNL_PVL, IST);
	setgate (IDT_GP, (phys_addr_t)&general_protection,  IDT_TRAP_GATE, KNL_PVL, IST);
	setgate (IDT_PF, (phys_addr_t)&page_fault,  IDT_TRAP_GATE, KNL_PVL, IST);
	setgate (IDT_MF, (phys_addr_t)&fp_exception,  IDT_TRAP_GATE, KNL_PVL, IST);
	setgate (IDT_AC, (phys_addr_t)&alignment_check, IDT_TRAP_GATE, KNL_PVL, IST);
	setgate (IDT_MC, (phys_addr_t)&machine_check,  IDT_TRAP_GATE, KNL_PVL, IST);
	setgate (IDT_XF, (phys_addr_t)&xmm, IDT_TRAP_GATE, KNL_PVL, IST);
}

static void
idt_init (void)
{
  int vec_no;

	for (vec_no = 0; vec_no < NIDT; vec_no++)
		setgate (vec_no, (phys_addr_t)&reserved, IDT_TRAP_GATE, KNL_PVL, IST);

  trap_init ();

	idtr.dr_limit = sizeof(idt) - 1;
	idtr.dr_base = (phys_addr_t)&idt;
	lidt(&idtr);
}

void
interrupt_init (void)
{
  /*
   * Never ever think of putting lapic_init() or ioapic_init()
   * here. This function is going to be called by APs, so its not called
   * only once, but several time.
   */
  idt_init ();
}

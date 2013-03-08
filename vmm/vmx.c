#include <msr-index.h>
#include <vmx.h>
#include <panic.h>
#include <types.h>
#include <asmfunc.h>
#include <debug.h>
#include <cpu.h>
#include <string.h>
#include <cpuinfo.h>
#include <ipc.h>
#include <vuos/vuos.h>
/* ============================================= */
#define EIO 5
/* ============================================= */
static inline void
read_msr (uint32_t msr, uint32_t *val1, uint32_t *val2)
{
  uint64_t __val = rdmsr (msr);
  *val1 = (uint32_t)__val;
  *val2 = (uint32_t)(__val >> 32);
}
/* ============================================= */
static struct vmcs_config {
	int size;
	int order;
	uint32_t revision_id;
	uint32_t pin_based_exec_ctrl;
	uint32_t cpu_based_exec_ctrl;
	uint32_t cpu_based_2nd_exec_ctrl;
	uint32_t vmexit_ctrl;
	uint32_t vmentry_ctrl;
} vmcs_config;
/* ============================================= */
static struct vmx_capability {
	uint32_t ept;
	uint32_t vpid;
	int has_load_efer:1;
} vmx_capability;
/* ============================================= */
static int
adjust_vmx_controls (uint32_t ctl_min, uint32_t ctl_opt,
				             uint32_t msr, uint32_t *result)
{
	uint32_t vmx_msr_high;
	uint32_t vmx_msr_low;
	uint32_t ctl;

  ctl = ctl_min | ctl_opt;

	read_msr (msr, &vmx_msr_low, &vmx_msr_high);

	ctl &= vmx_msr_high; /* bit == 0 in high word ==> must be zero */
	ctl |= vmx_msr_low;  /* bit == 1 in low word  ==> must be one  */

	/* Ensure minimum (required) set of control bits are supported. */
	if (ctl_min & ~ctl)
		return -EIO;

	*result = ctl;
	return 0;
}
/* ============================================= */
static bool
allow_1_setting (uint32_t msr, uint32_t ctl)
{
	uint32_t vmx_msr_high;
	uint32_t vmx_msr_low;

	read_msr (msr, &vmx_msr_low, &vmx_msr_high);

	return vmx_msr_high & ctl;
}
/* ============================================= */
static inline bool
cpu_has_secondary_exec_ctrls (void)
{
	return vmcs_config.cpu_based_exec_ctrl &
		CPU_BASED_ACTIVATE_SECONDARY_CONTROLS;
}
/* ============================================= */
static inline bool
cpu_has_vmx_vpid (void)
{
	return vmcs_config.cpu_based_2nd_exec_ctrl &
		SECONDARY_EXEC_ENABLE_VPID;
}
/* ============================================= */
static inline bool
cpu_has_vmx_invvpid_single (void)
{
	return vmx_capability.vpid & VMX_VPID_EXTENT_SINGLE_CONTEXT_BIT;
}
/* ============================================= */
static inline bool
cpu_has_vmx_invvpid_global (void)
{
	return vmx_capability.vpid & VMX_VPID_EXTENT_GLOBAL_CONTEXT_BIT;
}
/* ============================================= */
static inline bool
cpu_has_vmx_ept (void)
{
	return vmcs_config.cpu_based_2nd_exec_ctrl &
		SECONDARY_EXEC_ENABLE_EPT;
}
/* ============================================= */
#if 0
static inline bool
cpu_has_vmx_invept_individual_addr (void)
{
	return vmx_capability.ept & VMX_EPT_EXTENT_INDIVIDUAL_BIT;
}
#endif
/* ============================================= */
static inline bool
cpu_has_vmx_invept_context (void)
{
	return vmx_capability.ept & VMX_EPT_EXTENT_CONTEXT_BIT;
}
/* ============================================= */
static inline bool
cpu_has_vmx_invept_global (void)
{
	return vmx_capability.ept & VMX_EPT_EXTENT_GLOBAL_BIT;
}
/* ============================================= */
static int
setup_vmcs_config (struct vmcs_config *vmcs_conf)
{
	uint32_t vmx_msr_low, vmx_msr_high;
	uint32_t min, opt, min2, opt2;
	uint32_t _pin_based_exec_control = 0;
	uint32_t _cpu_based_exec_control = 0;
	uint32_t _cpu_based_2nd_exec_control = 0;
	uint32_t _vmexit_control = 0;
	uint32_t _vmentry_control = 0;

	min = PIN_BASED_EXT_INTR_MASK | PIN_BASED_NMI_EXITING;
	opt = PIN_BASED_VIRTUAL_NMIS;
	if (adjust_vmx_controls (min, opt, MSR_IA32_VMX_PINBASED_CTLS,
			&_pin_based_exec_control) < 0)
		return -EIO;

	min = CPU_BASED_CR8_LOAD_EXITING |
	      CPU_BASED_CR8_STORE_EXITING |
	      CPU_BASED_CR3_LOAD_EXITING |
	      CPU_BASED_CR3_STORE_EXITING |
	      CPU_BASED_MOV_DR_EXITING |
	      CPU_BASED_USE_TSC_OFFSETING |
	      CPU_BASED_MWAIT_EXITING |
	      CPU_BASED_MONITOR_EXITING |
	      CPU_BASED_INVLPG_EXITING;

#if 0 /* FIXME: Do we need this? */
	if (yield_on_hlt)
		min |= CPU_BASED_HLT_EXITING;
#endif
	min |= CPU_BASED_HLT_EXITING;

	opt = CPU_BASED_TPR_SHADOW |
	      CPU_BASED_USE_MSR_BITMAPS |
	      CPU_BASED_ACTIVATE_SECONDARY_CONTROLS;
	if (adjust_vmx_controls (min, opt, MSR_IA32_VMX_PROCBASED_CTLS,
			&_cpu_based_exec_control) < 0)
		return -EIO;
	if ((_cpu_based_exec_control & CPU_BASED_TPR_SHADOW))
		   _cpu_based_exec_control &= ~CPU_BASED_CR8_LOAD_EXITING & ~CPU_BASED_CR8_STORE_EXITING;
	if (_cpu_based_exec_control & CPU_BASED_ACTIVATE_SECONDARY_CONTROLS) {
		min2 = 0;
		opt2 = //SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES |
			SECONDARY_EXEC_WBINVD_EXITING |
			SECONDARY_EXEC_ENABLE_VPID |
			SECONDARY_EXEC_ENABLE_EPT |
//			SECONDARY_EXEC_UNRESTRICTED_GUEST |
//			SECONDARY_EXEC_PAUSE_LOOP_EXITING |
			SECONDARY_EXEC_RDTSCP;
		if (adjust_vmx_controls (min2, opt2, MSR_IA32_VMX_PROCBASED_CTLS2,
				&_cpu_based_2nd_exec_control) < 0)
			return -EIO;
	}
	if (!(_cpu_based_2nd_exec_control & SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES))
		  _cpu_based_exec_control &= ~CPU_BASED_TPR_SHADOW;
	if (_cpu_based_2nd_exec_control & SECONDARY_EXEC_ENABLE_EPT) {
		/* CR3 accesses and invlpg don't need to cause VM Exits when EPT
		   enabled */
		_cpu_based_exec_control &= ~(CPU_BASED_CR3_LOAD_EXITING |
                                 CPU_BASED_CR3_STORE_EXITING |
                                 CPU_BASED_INVLPG_EXITING);
		read_msr (MSR_IA32_VMX_EPT_VPID_CAP,
		      &vmx_capability.ept, &vmx_capability.vpid);
	}

	min = 0;
	min |= VM_EXIT_HOST_ADDR_SPACE_SIZE;
//	opt = VM_EXIT_SAVE_IA32_PAT | VM_EXIT_LOAD_IA32_PAT;
	opt = 0;
	if (adjust_vmx_controls(min, opt, MSR_IA32_VMX_EXIT_CTLS,
				&_vmexit_control) < 0)
		return -EIO;

	min = 0;
//	opt = VM_ENTRY_LOAD_IA32_PAT;
	opt = 0;
	if (adjust_vmx_controls(min, opt, MSR_IA32_VMX_ENTRY_CTLS,
				&_vmentry_control) < 0)
		return -EIO;

	read_msr (MSR_IA32_VMX_BASIC, &vmx_msr_low, &vmx_msr_high);

	/* IA-32 SDM Vol 3B: VMCS size is never greater than 4kB. */
	if ((vmx_msr_high & 0x1fff) > _4KB_)
		return -EIO;

	/* IA-32 SDM Vol 3B: 64-bit CPUs always have VMX_BASIC_MSR[48]==0. */
	if (vmx_msr_high & (1 << 16))
		return -EIO;

	/* Require Write-Back (WB) memory type for VMCS accesses. */
	if (((vmx_msr_high >> 18) & 15) != 6)
		return -EIO;

	vmcs_conf->size = vmx_msr_high & 0x1fff;
	//vmcs_conf->order = get_order(vmcs_config.size);
	vmcs_conf->revision_id = vmx_msr_low;

	vmcs_conf->pin_based_exec_ctrl = _pin_based_exec_control;
	vmcs_conf->cpu_based_exec_ctrl = _cpu_based_exec_control;
	vmcs_conf->cpu_based_2nd_exec_ctrl = _cpu_based_2nd_exec_control;
	vmcs_conf->vmexit_ctrl         = _vmexit_control;
	vmcs_conf->vmentry_ctrl        = _vmentry_control;

	vmx_capability.has_load_efer =
		allow_1_setting (MSR_IA32_VMX_ENTRY_CTLS, VM_ENTRY_LOAD_IA32_EFER) &&
    allow_1_setting (MSR_IA32_VMX_EXIT_CTLS, VM_EXIT_LOAD_IA32_EFER);

	return 0;
}
/* ============================================= */
static void
enable_cr4_vme ()
{
	uint64_t cr4 = rcr4 ();

	cr4 |= (1 << 13);
	lcr4(cr4);

	cr4 = rcr4();
}
static void msr_lock()
{
	uint64_t nl = 0;

	nl = rdmsr(MSR_IA32_FEATURE_CONTROL);

	if (!(nl & 1)) {
		nl |= 1;
		nl |= 2;
		wrmsr(MSR_IA32_FEATURE_CONTROL, nl);
	} else {
		DEBUG("MSR Already locked\n", 0xB);
	}
}

static void
__vmxon (phys_addr_t vmxon_ptr)
{
	__asm__ __volatile__ (ASM_VMX_VMXON_RAX /* VMXON %0 */
			: : "a"(&vmxon_ptr), "m"(vmxon_ptr)
			: "memory", "cc");
}

static void
vmxon (phys_addr_t vmcs_ptr, phys_addr_t vmxon_ptr)
{
  uint32_t vmcs_revision_id;
  uint32_t vmx_abort_indicator = 0;
  uint32_t *ptr;

	vmcs_revision_id = rdmsr(MSR_IA32_VMX_BASIC);

  memset ((uint8_t*)vmxon_ptr, 0, 4096);
  memset ((uint8_t*)vmcs_ptr, 0, 4096);

  ptr = (uint32_t *)vmxon_ptr;
  ptr[0] = vmcs_revision_id;
  ptr[1] = vmx_abort_indicator;

  ptr = (uint32_t *)vmcs_ptr;
  ptr[0] = vmcs_revision_id;
  ptr[1] = vmx_abort_indicator;

	__vmxon(vmxon_ptr);
}

static void
enable_cr0 (void)
{
	uint64_t cr0 = rcr0();

	cr0 |= (1 << 5);
	lcr0(cr0);

	cr0 = rcr0();
}

static void
clear_vmcs (phys_addr_t vmcs_ptr)
{
	uint8_t error;

	__asm__ __volatile__ (ASM_VMX_VMCLEAR_RAX "; setna %0"
			: "=qm"(error) : "a"(&vmcs_ptr), "m"(vmcs_ptr)
			: "cc", "memory");

	if (error)
		panic("vmclear fail");
}

static void
load_vmcs (phys_addr_t vmcs_ptr)
{
	uint8_t error;

	__asm__ __volatile__ (ASM_VMX_VMPTRLD_RAX "; setna %0"
			: "=qm"(error) : "a"(&vmcs_ptr), "m"(vmcs_ptr)
			: "cc", "memory");

	if (error)
		panic("vmclear fail");
}

static uint64_t
vmx_vmread (uint64_t field)
{
	uint64_t value;

	__asm__ __volatile__ (ASM_VMX_VMREAD_RDX_RAX: "=a"(value): "d"(field) :"cc");

	return value;
}

static void
vmx_vmwrite (uint64_t field, uint64_t value)
{
	uint8_t error;

	__asm__ __volatile__ (ASM_VMX_VMWRITE_RAX_RDX "; setna %0"
			: "=q"(error) : "a"(value), "d"(field) : "cc");

	if (error) {
		panic ("Error executing VMXWRITE(%d, %d)", 0x4, field, value);
	}
}

static void host_entry (void);

static void
setup_vmcs (void)
{
  struct descriptor_register *gdtr = NULL;
  sgdt(gdtr);

  struct descriptor_register *idtr = NULL;
  sidt(idtr);

  uint16_t u16 = 0;
  uint64_t u64 = 0;

  // 16-Bit Host-State Fields
  __asm__ __volatile__("mov %%es, %0" : "=m" (u16));	vmx_vmwrite(HOST_ES_SELECTOR, u16);
  __asm__ __volatile__("mov %%cs, %0" : "=m" (u16));	vmx_vmwrite(HOST_CS_SELECTOR, u16);
  __asm__ __volatile__("mov %%ss, %0" : "=m" (u16));	vmx_vmwrite(HOST_SS_SELECTOR, u16);
  __asm__ __volatile__("mov %%ds, %0" : "=m" (u16));	vmx_vmwrite(HOST_DS_SELECTOR, u16);
  __asm__ __volatile__("mov %%fs, %0" : "=m" (u16));	vmx_vmwrite(HOST_FS_SELECTOR, u16);
  __asm__ __volatile__("mov %%gs, %0" : "=m" (u16));	vmx_vmwrite(HOST_GS_SELECTOR, u16);
  /*__asm__ __volatile__("str %0" : "=m" (u16));*/		vmx_vmwrite(HOST_TR_SELECTOR, 0x18);

  // 64-Bit Guest-State Fields
  vmx_vmwrite(VMCS_LINK_POINTER, 0xFFFFFFFF);
  vmx_vmwrite(VMCS_LINK_POINTER_HIGH, 0xFFFFFFFF);

#if 0
  /*
   * We have this problem with Bochs:
   * "RDMSR: Unknown register 0x1d9"
   */
  u64 = rdmsr(MSR_IA32_DEBUGCTLMSR);
  vmx_vmwrite(GUEST_IA32_DEBUGCTL, u64);
  vmx_vmwrite(GUEST_IA32_DEBUGCTL_HIGH, u64 >> 32);
#endif

  // 32-Bit Control Fields
  // According to our new findings (0-settings and 1-settings in spec Appendix A.3.1)
  // Section 24.6.1 indicates the action can be controlled by each bit in pin_base_control
  uint64_t vmx_pinbase = rdmsr (MSR_IA32_VMX_PINBASED_CTLS);

  u64 = (uint32_t)vmx_pinbase; /* Get just the first 32bit of the MSR, because those are the 0-settings */
  //u64 |= 1;                    /* If the bit 0 is set, external interrupts cause a VMEXIT */
  vmx_vmwrite(PIN_BASED_VM_EXEC_CONTROL, u64);

  // Section 24.6.2 Table 24-6
  // Appendix A3.2 definition of actions can be controled by setting bits
  uint64_t vmx_primary_processor = rdmsr (MSR_IA32_VMX_PROCBASED_CTLS);

  u64 = (uint32_t)vmx_primary_processor;
  u64 |= (1 << 31);                        /* Enable the secondary controls */
  vmx_vmwrite(CPU_BASED_VM_EXEC_CONTROL, u64);

  // Section 24.6.2 Table 24-7
  uint64_t vmx_secondary_processor= rdmsr (MSR_IA32_VMX_PROCBASED_CTLS2);

  u64 = (uint32_t)vmx_secondary_processor;
  u64 |= (1 << 1);                         /* Enable the EPT */
  vmx_vmwrite(SECONDARY_VM_EXEC_CONTROL, u64);

  // Section 24.7.1 Table 24-10
  uint64_t vmx_exit_controls = rdmsr (MSR_IA32_VMX_EXIT_CTLS);

  u64 = (uint32_t)vmx_exit_controls;
  u64 |= (1 << 9);                         /* Enable the 64-bit mode after the VMEXIT */
  vmx_vmwrite(VM_EXIT_CONTROLS, u64);

  // Section 24.8.1 Table 24-12
  uint64_t vmx_entry_controls= rdmsr (MSR_IA32_VMX_ENTRY_CTLS);
  u64 = (uint32_t)vmx_entry_controls;
  u64 |= (1 << 9);                         /* Enable the IA-32e mode for the guest */
  vmx_vmwrite (VM_ENTRY_CONTROLS, u64);

  vmx_vmwrite (VM_EXIT_MSR_STORE_COUNT, 0);
  vmx_vmwrite (VM_EXIT_MSR_LOAD_COUNT, 0);
  vmx_vmwrite (VM_ENTRY_MSR_LOAD_COUNT, 0);
  vmx_vmwrite (VM_ENTRY_INTR_INFO_FIELD, 0);

  vmx_vmwrite (TSC_OFFSET, 0);
  vmx_vmwrite (TSC_OFFSET_HIGH, 0);
  vmx_vmwrite (PAGE_FAULT_ERROR_CODE_MASK, 0);
  vmx_vmwrite (PAGE_FAULT_ERROR_CODE_MATCH, 0);
  vmx_vmwrite (CR3_TARGET_COUNT, 0);
  vmx_vmwrite (CR3_TARGET_VALUE0, 0);
  vmx_vmwrite (CR3_TARGET_VALUE1, 0);
  vmx_vmwrite (CR3_TARGET_VALUE2, 0);
  vmx_vmwrite (CR3_TARGET_VALUE3, 0);

  // 16-Bit Guest-State Fields
  __asm__ __volatile__ ("mov %%es, %0" : "=m" (u16));	vmx_vmwrite(GUEST_ES_SELECTOR, u16);
  __asm__ __volatile__ ("mov %%cs, %0" : "=m" (u16));	vmx_vmwrite(GUEST_CS_SELECTOR, u16);
  __asm__ __volatile__ ("mov %%ss, %0" : "=m" (u16));	vmx_vmwrite(GUEST_SS_SELECTOR, u16);
  __asm__ __volatile__ ("mov %%ds, %0" : "=m" (u16));	vmx_vmwrite(GUEST_DS_SELECTOR, u16);
  __asm__ __volatile__ ("mov %%fs, %0" : "=m" (u16));	vmx_vmwrite(GUEST_FS_SELECTOR, u16);
  __asm__ __volatile__ ("mov %%gs, %0" : "=m" (u16));	vmx_vmwrite(GUEST_GS_SELECTOR, u16);
  __asm__ __volatile__ ("sldt %0" : "=m" (u16));		vmx_vmwrite(GUEST_LDTR_SELECTOR, u16);
  /*asm("str %0" : "=m" (u16));*/		vmx_vmwrite(GUEST_TR_SELECTOR, 0x18);

  // 32-Bit Guest-State Fields

  // NB: 3A 2.4.4 On power up or reset of the processor, the base address is set to the
  // default value of 0 and the limit is set to 0FFFFH.
  vmx_vmwrite (GUEST_CS_LIMIT, 0XFFFFFFFF);
  vmx_vmwrite (GUEST_ES_LIMIT, 0XFFFFFFFF);
  vmx_vmwrite (GUEST_SS_LIMIT, 0XFFFFFFFF);
  vmx_vmwrite (GUEST_DS_LIMIT, 0XFFFFFFFF);
  vmx_vmwrite (GUEST_FS_LIMIT, 0XFFFFFFFF);
  vmx_vmwrite (GUEST_GS_LIMIT, 0XFFFFFFFF);
  vmx_vmwrite (GUEST_LDTR_LIMIT, 0xFFFF);
  vmx_vmwrite (GUEST_TR_LIMIT, 0xFF);

  vmx_vmwrite (GUEST_GDTR_LIMIT, 0);
  vmx_vmwrite (GUEST_IDTR_LIMIT, 0);

  vmx_vmwrite (GUEST_DR7, 0);

  vmx_vmwrite (GUEST_INTERRUPTIBILITY_INFO, 0);
  vmx_vmwrite (GUEST_ACTIVITY_STATE, 0);

  vmx_vmwrite (GUEST_CS_AR_BYTES, 0xA09B);
  vmx_vmwrite (GUEST_ES_AR_BYTES, 0xC093);
  vmx_vmwrite (GUEST_SS_AR_BYTES, 0xC093);
  vmx_vmwrite (GUEST_DS_AR_BYTES, 0xC093);
  vmx_vmwrite (GUEST_FS_AR_BYTES, 0xC093);
  vmx_vmwrite (GUEST_GS_AR_BYTES, 0xC093);
  vmx_vmwrite (GUEST_LDTR_AR_BYTES, 0x00010000);
  vmx_vmwrite (GUEST_TR_AR_BYTES, 0x8b);

  // Natural-Width Guest-State Fields
  u64 = rdmsr (MSR_IA32_SYSENTER_CS);
  vmx_vmwrite (GUEST_SYSENTER_CS, u64);

  vmx_vmwrite (GUEST_CR0, rcr0());
  vmx_vmwrite (GUEST_CR3, cpuinfo->vm_info.vm_page_tables);
  vmx_vmwrite (GUEST_CR4, rcr4());

  vmx_vmwrite (GUEST_ES_BASE, 0);
  vmx_vmwrite (GUEST_CS_BASE, 0);
  vmx_vmwrite (GUEST_SS_BASE, 0);
  vmx_vmwrite (GUEST_DS_BASE, 0);
  vmx_vmwrite (GUEST_FS_BASE, 0);
  vmx_vmwrite (GUEST_GS_BASE, 0);
  vmx_vmwrite (GUEST_LDTR_BASE, 0);
  vmx_vmwrite (GUEST_TR_BASE, 0);

  vmx_vmwrite (GUEST_GDTR_BASE, 0);
  vmx_vmwrite (GUEST_IDTR_BASE, 0);

  vmx_vmwrite (EPT_POINTER, cpuinfo->vm_info.vm_ept | 0x6 | (3 << 3));

  vmx_vmwrite (GUEST_RFLAGS, cpuinfo->vm_info.vm_regs.rflags);

  u64 = rdmsr (MSR_IA32_SYSENTER_ESP);
  vmx_vmwrite (GUEST_SYSENTER_ESP, u64);
  u64 = rdmsr (MSR_IA32_SYSENTER_EIP);
  vmx_vmwrite (GUEST_SYSENTER_EIP, u64);

  // Natural- Width Host-State Fields
  vmx_vmwrite (HOST_CR0, rcr0());
  vmx_vmwrite (HOST_CR3, rcr3());
  vmx_vmwrite (HOST_CR4, rcr4());

  vmx_vmwrite (HOST_FS_BASE, 0);
  vmx_vmwrite (HOST_GS_BASE, 0);
  vmx_vmwrite (HOST_TR_BASE, 0);
  vmx_vmwrite (HOST_GDTR_BASE, gdtr->dr_base);
  vmx_vmwrite (HOST_IDTR_BASE, idtr->dr_base);

  vmx_vmwrite (GUEST_RSP, cpuinfo->vm_info.vm_regs.rsp);
  vmx_vmwrite (GUEST_RIP, (uint64_t)cpuinfo->vm_info.vm_regs.rip);

  vmx_vmwrite (HOST_RSP, cpuinfo->vmm_info.vmm_regs.rsp);
  vmx_vmwrite (HOST_RIP, (uint64_t)host_entry);
}

static void
vmresume (void)
{
  __asm__ __volatile__ (
      "movq %c[VM_REGS_RAX_IDX](%0), %%rax\n\t"
      "movq %c[VM_REGS_RBX_IDX](%0), %%rbx\n\t"
      "movq %c[VM_REGS_RCX_IDX](%0), %%rcx\n\t"
      "movq %c[VM_REGS_RDX_IDX](%0), %%rdx\n\t"
      "movq %c[VM_REGS_RSI_IDX](%0), %%rsi\n\t"
      "movq %c[VM_REGS_RBP_IDX](%0), %%rbp\n\t"
      "movq %c[VM_REGS_R8_IDX](%0), %%r8\n\t"
      "movq %c[VM_REGS_R9_IDX](%0), %%r9\n\t"
      "movq %c[VM_REGS_R10_IDX](%0), %%r10\n\t"
      "movq %c[VM_REGS_R11_IDX](%0), %%r11\n\t"
      "movq %c[VM_REGS_R12_IDX](%0), %%r12\n\t"
      "movq %c[VM_REGS_R13_IDX](%0), %%r13\n\t"
      "movq %c[VM_REGS_R14_IDX](%0), %%r14\n\t"
      "movq %c[VM_REGS_R15_IDX](%0), %%r15\n\t"
      "movq %c[VM_REGS_RDI_IDX](%0), %%rdi\n\t"
      ASM_VMX_VMRESUME
      :
      :
      "D"(cpuinfo),
      [VM_REGS_RAX_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.rax)),
      [VM_REGS_RBX_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.rbx)),
      [VM_REGS_RCX_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.rcx)),
      [VM_REGS_RDX_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.rdx)),
      [VM_REGS_RSI_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.rsi)),
      [VM_REGS_RBP_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.rbp)),
      [VM_REGS_R8_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.r8)),
      [VM_REGS_R9_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.r9)),
      [VM_REGS_R10_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.r10)),
      [VM_REGS_R11_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.r11)),
      [VM_REGS_R12_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.r12)),
      [VM_REGS_R13_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.r13)),
      [VM_REGS_R14_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.r14)),
      [VM_REGS_R15_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.r15)),
      [VM_REGS_RDI_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.rdi))
  );
}

static void
vmlaunch (void)
{
  __asm__ __volatile__ (
      "movq %c[VM_REGS_RAX_IDX](%0), %%rax\n\t"
      "movq %c[VM_REGS_RBX_IDX](%0), %%rbx\n\t"
      "movq %c[VM_REGS_RCX_IDX](%0), %%rcx\n\t"
      "movq %c[VM_REGS_RDX_IDX](%0), %%rdx\n\t"
      "movq %c[VM_REGS_RSI_IDX](%0), %%rsi\n\t"
      "movq %c[VM_REGS_RBP_IDX](%0), %%rbp\n\t"
      "movq %c[VM_REGS_R8_IDX](%0), %%r8\n\t"
      "movq %c[VM_REGS_R9_IDX](%0), %%r9\n\t"
      "movq %c[VM_REGS_R10_IDX](%0), %%r10\n\t"
      "movq %c[VM_REGS_R11_IDX](%0), %%r11\n\t"
      "movq %c[VM_REGS_R12_IDX](%0), %%r12\n\t"
      "movq %c[VM_REGS_R13_IDX](%0), %%r13\n\t"
      "movq %c[VM_REGS_R14_IDX](%0), %%r14\n\t"
      "movq %c[VM_REGS_R15_IDX](%0), %%r15\n\t"
      "movq %c[VM_REGS_RDI_IDX](%0), %%rdi\n\t"
      ASM_VMX_VMLAUNCH
      :
      :
      "D"(cpuinfo),
      [VM_REGS_RAX_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.rax)),
      [VM_REGS_RBX_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.rbx)),
      [VM_REGS_RCX_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.rcx)),
      [VM_REGS_RDX_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.rdx)),
      [VM_REGS_RSI_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.rsi)),
      [VM_REGS_RBP_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.rbp)),
      [VM_REGS_R8_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.r8)),
      [VM_REGS_R9_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.r9)),
      [VM_REGS_R10_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.r10)),
      [VM_REGS_R11_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.r11)),
      [VM_REGS_R12_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.r12)),
      [VM_REGS_R13_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.r13)),
      [VM_REGS_R14_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.r14)),
      [VM_REGS_R15_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.r15)),
      [VM_REGS_RDI_IDX]"i"(offsetof (struct cpu_info, vm_info.vm_regs.rdi))
  );
}

static void
host_entry (void)
{
	uint64_t reason = vmx_vmread (VM_EXIT_REASON);
  phys_addr_t msg_data;

	if (reason == 18) {
		msg_receive(PM);

		msg_data = (phys_addr_t)cpuinfo->msg_input[PM].data;

		int r = *(int*)msg_data;

		if (r != -1) {
			clear_vmcs (cpuinfo->vm_info.vm_vmcs_ptr);
		}

		load_vmcs (cpuinfo->vm_info.vm_vmcs_ptr);

		setup_vmcs ();

		if (r == -1) {
			vmresume();
		} else {
			vmlaunch ();
		}
	} else {
		DEBUG ("HOOOOOOOOOSSSSSSSSSTTTTTTTTT!!!!!!!!!!!!!\n", 0xA);
		DEBUG ("Exited because of %d\n", 0x4, reason);
		while(1);
	}
}

static void
check_cpu_vmx_features (struct vmcs_config *vmcs_config)
{
  if (setup_vmcs_config (vmcs_config) < 0) {
    panic ("setup_vmcs_config failed!");
  }
  if (! cpu_has_vmx_ept ()) {
    panic ("Your cpu does not support ept!");
  }
  if (! cpu_has_secondary_exec_ctrls ()) {
    panic ("Your cpu does not support secondary exec controls");
  }
  if (! cpu_has_vmx_invept_global ()) {
    panic ("Your cpu does not support invept global");
  }
  if (! cpu_has_vmx_invvpid_single ()) {
    panic ("Your cpu does not support invpid single");
  }
}
void
vmx_init (void)
{
  check_cpu_vmx_features (&vmcs_config);
	/* TODO:
	 * Make Sure A20 line is disabled here
	 */
	enable_cr4_vme();

	enable_cr0();

	msr_lock();

	vmxon(cpuinfo->vm_info.vm_vmcs_ptr, cpuinfo->vm_info.vm_vmxon_ptr);

	clear_vmcs (cpuinfo->vm_info.vm_vmcs_ptr);

	load_vmcs (cpuinfo->vm_info.vm_vmcs_ptr);

	setup_vmcs ();

	vmlaunch ();

	panic("We MUST NEVER get here.");
}

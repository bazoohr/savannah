#include <msr-index.h>
#include <vmx.h>
#include <panic.h>
#include <types.h>
#include <asmfunc.h>
#include <printk.h>
#include <cpu.h>
#include <string.h>

static int cpu_has_vmx()
{
	uint32_t ecx;
	cpuid (1, NULL, NULL, &ecx, NULL);
	return (ecx & (1 << 5));
}

static int cpu_has_msr()
{
	uint32_t edx;
	cpuid (1, NULL, NULL, NULL, &edx);
	return (edx & (1 << 5));
}

static void enable_cr4_vme()
{
	uint64_t cr4 = rcr4();

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
		cprintk("MSR Already locked\n", 0xB);
	}
}

static void __vmxon(phys_addr_t vmxon_ptr)
{
	asm volatile (ASM_VMX_VMXON_RAX /* VMXON %0 */
			: : "a"(&vmxon_ptr), "m"(vmxon_ptr)
			: "memory", "cc");
}

static void vmxon(phys_addr_t vmcs_ptr, phys_addr_t vmxon_ptr)
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

static void enable_cr0()
{
	uint64_t cr0 = rcr0();

	cr0 |= (1 << 5);
	lcr0(cr0);

	cr0 = rcr0();
}

static void clear_vmcs(phys_addr_t vmcs_ptr)
{
	uint8_t error;

	asm volatile (ASM_VMX_VMCLEAR_RAX "; setna %0"
			: "=qm"(error) : "a"(&vmcs_ptr), "m"(vmcs_ptr)
			: "cc", "memory");

	if (error)
		panic("vmclear fail");
}

static void load_vmcs(phys_addr_t vmcs_ptr)
{
	uint8_t error;

	asm volatile (ASM_VMX_VMPTRLD_RAX "; setna %0"
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

static void vmx_vmwrite(uint64_t field, uint64_t value)
{
	uint8_t error;

	asm volatile (ASM_VMX_VMWRITE_RAX_RDX "; setna %0"
			: "=q"(error) : "a"(value), "d"(field) : "cc");

	if (error) {
		cprintk("Error executing VMXWRITE(%d, %d)", 0x4, field, value);
		panic("");
	}
}

void host_entry();

static void setup_vmcs(struct cpu_info *cpuinfo)
{
	struct descriptor_register *gdtr = NULL;
	sgdt(gdtr);

	struct descriptor_register *idtr = NULL;
	sidt(idtr);

	uint16_t u16 = 0;
	uint64_t u64 = 0;
	uint64_t flags = 0;

	// 16-Bit Host-State Fields
	asm("mov %%es, %0" : "=m" (u16));	vmx_vmwrite(HOST_ES_SELECTOR, u16);
	asm("mov %%cs, %0" : "=m" (u16));	vmx_vmwrite(HOST_CS_SELECTOR, u16);
	asm("mov %%ss, %0" : "=m" (u16));	vmx_vmwrite(HOST_SS_SELECTOR, u16);
	asm("mov %%ds, %0" : "=m" (u16));	vmx_vmwrite(HOST_DS_SELECTOR, u16);
	asm("mov %%fs, %0" : "=m" (u16));	vmx_vmwrite(HOST_FS_SELECTOR, u16);
	asm("mov %%gs, %0" : "=m" (u16));	vmx_vmwrite(HOST_GS_SELECTOR, u16);
	/*asm("str %0" : "=m" (u16));*/		vmx_vmwrite(HOST_TR_SELECTOR, 0x18);

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
	u64 |= 1;                    /* If the bit 0 is set, external interrupts cause a VMEXIT */
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
	vmx_vmwrite(VM_ENTRY_CONTROLS, u64);

	vmx_vmwrite(VM_EXIT_MSR_STORE_COUNT, 0);
	vmx_vmwrite(VM_EXIT_MSR_LOAD_COUNT, 0);
	vmx_vmwrite(VM_ENTRY_MSR_LOAD_COUNT, 0);
	vmx_vmwrite(VM_ENTRY_INTR_INFO_FIELD, 0);

	vmx_vmwrite(TSC_OFFSET, 0);
	vmx_vmwrite(TSC_OFFSET_HIGH, 0);
	vmx_vmwrite(PAGE_FAULT_ERROR_CODE_MASK, 0);
	vmx_vmwrite(PAGE_FAULT_ERROR_CODE_MATCH, 0);
	vmx_vmwrite(CR3_TARGET_COUNT, 0);
	vmx_vmwrite(CR3_TARGET_VALUE0, 0);
	vmx_vmwrite(CR3_TARGET_VALUE1, 0);
	vmx_vmwrite(CR3_TARGET_VALUE2, 0);
	vmx_vmwrite(CR3_TARGET_VALUE3, 0);

	// 16-Bit Guest-State Fields
	asm("mov %%es, %0" : "=m" (u16));	vmx_vmwrite(GUEST_ES_SELECTOR, u16);
	asm("mov %%cs, %0" : "=m" (u16));	vmx_vmwrite(GUEST_CS_SELECTOR, u16);
	asm("mov %%ss, %0" : "=m" (u16));	vmx_vmwrite(GUEST_SS_SELECTOR, u16);
	asm("mov %%ds, %0" : "=m" (u16));	vmx_vmwrite(GUEST_DS_SELECTOR, u16);
	asm("mov %%fs, %0" : "=m" (u16));	vmx_vmwrite(GUEST_FS_SELECTOR, u16);
	asm("mov %%gs, %0" : "=m" (u16));	vmx_vmwrite(GUEST_GS_SELECTOR, u16);
	asm("sldt %0" : "=m" (u16));		vmx_vmwrite(GUEST_LDTR_SELECTOR, u16);
	/*asm("str %0" : "=m" (u16));*/		vmx_vmwrite(GUEST_TR_SELECTOR, 0x18);

	// 32-Bit Guest-State Fields

	// NB: 3A 2.4.4 On power up or reset of the processor, the base address is set to the
	// default value of 0 and the limit is set to 0FFFFH.
	vmx_vmwrite(GUEST_CS_LIMIT, 0XFFFFFFFF);
	vmx_vmwrite(GUEST_ES_LIMIT, 0XFFFFFFFF);
	vmx_vmwrite(GUEST_SS_LIMIT, 0XFFFFFFFF);
	vmx_vmwrite(GUEST_DS_LIMIT, 0XFFFFFFFF);
	vmx_vmwrite(GUEST_FS_LIMIT, 0XFFFFFFFF);
	vmx_vmwrite(GUEST_GS_LIMIT, 0XFFFFFFFF);
	vmx_vmwrite(GUEST_LDTR_LIMIT, 0xFFFF);
	vmx_vmwrite(GUEST_TR_LIMIT, 0xFF);

	vmx_vmwrite(GUEST_GDTR_LIMIT, gdtr->dr_limit);
	vmx_vmwrite(GUEST_IDTR_LIMIT, idtr->dr_limit);

	vmx_vmwrite(GUEST_DR7, 0);

	vmx_vmwrite(GUEST_INTERRUPTIBILITY_INFO, 0);
	vmx_vmwrite(GUEST_ACTIVITY_STATE, 0);

	vmx_vmwrite(GUEST_CS_AR_BYTES, 0xA09B);
	vmx_vmwrite(GUEST_ES_AR_BYTES, 0xC093);
	vmx_vmwrite(GUEST_SS_AR_BYTES, 0xC093);
	vmx_vmwrite(GUEST_DS_AR_BYTES, 0xC093);
	vmx_vmwrite(GUEST_FS_AR_BYTES, 0xC093);
	vmx_vmwrite(GUEST_GS_AR_BYTES, 0xC093);
	vmx_vmwrite(GUEST_LDTR_AR_BYTES, 0x00010000);
	vmx_vmwrite(GUEST_TR_AR_BYTES, 0x8b);

	// Natural-Width Guest-State Fields
	u64 = rdmsr(MSR_IA32_SYSENTER_CS);
	vmx_vmwrite(GUEST_SYSENTER_CS, u64);

	vmx_vmwrite(GUEST_CR0, rcr0());
	vmx_vmwrite(GUEST_CR3, cpuinfo->vm_info.vm_page_tables);
	vmx_vmwrite(GUEST_CR4, rcr4());

	vmx_vmwrite(GUEST_ES_BASE, 0);
	vmx_vmwrite(GUEST_CS_BASE, 0);
	vmx_vmwrite(GUEST_SS_BASE, 0);
	vmx_vmwrite(GUEST_DS_BASE, 0);
	vmx_vmwrite(GUEST_FS_BASE, 0);
	vmx_vmwrite(GUEST_GS_BASE, 0);
	vmx_vmwrite(GUEST_LDTR_BASE, 0);
	vmx_vmwrite(GUEST_TR_BASE, 0);

	vmx_vmwrite(GUEST_GDTR_BASE, gdtr->dr_base);
	vmx_vmwrite(GUEST_IDTR_BASE, idtr->dr_base);

	vmx_vmwrite(EPT_POINTER, cpuinfo->vm_info.vm_ept | 0x6 | (3 << 3));

	flags = rflags();
	vmx_vmwrite(GUEST_RFLAGS, flags);

	u64 = rdmsr(MSR_IA32_SYSENTER_ESP);
	vmx_vmwrite(GUEST_SYSENTER_ESP, u64);
	u64 = rdmsr(MSR_IA32_SYSENTER_EIP);
	vmx_vmwrite(GUEST_SYSENTER_EIP, u64);

	// Natural-Width Host-State Fields
	vmx_vmwrite(HOST_CR0, rcr0());
	vmx_vmwrite(HOST_CR3, rcr3());
	vmx_vmwrite(HOST_CR4, rcr4());

	vmx_vmwrite(HOST_FS_BASE, 0);
	vmx_vmwrite(HOST_GS_BASE, 0);
	vmx_vmwrite(HOST_TR_BASE, 0);
	vmx_vmwrite(HOST_GDTR_BASE, gdtr->dr_base);
	vmx_vmwrite(HOST_IDTR_BASE, idtr->dr_base);

	vmx_vmwrite(GUEST_RSP, cpuinfo->vm_info.vm_stack_vaddr);
	vmx_vmwrite(GUEST_RIP, (uint64_t)cpuinfo->vm_info.vm_start_vaddr);

	vmx_vmwrite(HOST_RSP, cpuinfo->vmm_info.vmm_stack_vaddr);
	vmx_vmwrite(HOST_RIP, (uint64_t)host_entry);
}

void vmresume()
{
	__asm__ volatile (ASM_VMX_VMRESUME);
}

void host_entry()
{
	cprintk("HOOOOOOOOOSSSSSSSSSTTTTTTTTT!!!!!!!!!!!!!\n", 0xA);
	uint64_t reason = vmx_vmread (VM_EXIT_REASON);
	cprintk ("Exited because of %d\n", 0xF, reason);
	//vmresume();
	while(1);
}

void vmlaunch()
{
	__asm__ volatile (ASM_VMX_VMLAUNCH);
}

void vmx_init(struct cpu_info *cpuinfo)
{
	/* TODO:
	 * Make Sure A20 line is disabled here
	 */
	if (!cpu_has_vmx()) {
		panic("CPU does not support VT-x");
	}

	if (!cpu_has_msr()) {
		panic("CPU does not support MSR");
	}

	enable_cr4_vme();

	enable_cr0();

	msr_lock();

	vmxon(cpuinfo->vm_info.vm_vmcs_ptr, cpuinfo->vm_info.vm_vmxon_ptr);

	clear_vmcs (cpuinfo->vm_info.vm_vmcs_ptr);

	load_vmcs (cpuinfo->vm_info.vm_vmcs_ptr);

	setup_vmcs (cpuinfo);

	vmlaunch ();

	panic("We MUST NEVER get here.");
}

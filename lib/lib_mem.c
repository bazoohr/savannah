#include <lib_mem.h>
#include <debug.h>
#include <asmfunc.h>
#include <panic.h>
phys_addr_t
virt2phys (struct cpu_info *cpuinfo, virt_addr_t vaddr)
{
  phys_addr_t paddr;
  size_t offset;
#if 0
  DEBUG ("id = %d vm_start = %x vm_end = %x\n", 0xA, cpuinfo->cpuid, cpuinfo->vm_info.vm_start_vaddr,
      cpuinfo->vm_info.vm_end_vaddr);
  DEBUG ("vm_start = %x vm_end = %x\n", 0xA, cpuinfo->vm_info.vm_start_paddr,
      cpuinfo->vm_info.vm_end_paddr);
  DEBUG ("vaddr = %x\n", 0x4, vaddr);
#endif
  if (vaddr >= cpuinfo->vm_info.vm_code_vaddr &&
      vaddr < cpuinfo->vm_info.vm_data_vaddr) {
    offset = vaddr - cpuinfo->vm_info.vm_code_vaddr;
    paddr = cpuinfo->vm_info.vm_code_paddr + offset;
    return paddr;
  } else if (vaddr >= cpuinfo->vm_info.vm_data_vaddr &&
             vaddr < cpuinfo->vm_info.vm_rodata_vaddr) {
    offset = vaddr - cpuinfo->vm_info.vm_data_vaddr;
    paddr = cpuinfo->vm_info.vm_data_paddr + offset;
    return paddr;
  } else if (vaddr >= cpuinfo->vm_info.vm_rodata_vaddr &&
             vaddr < cpuinfo->vm_info.vm_bss_vaddr) {
    offset = vaddr - cpuinfo->vm_info.vm_rodata_vaddr;
    paddr = cpuinfo->vm_info.vm_rodata_paddr + offset;
    return paddr;
  } else if (vaddr >= cpuinfo->vm_info.vm_bss_vaddr &&
             vaddr < cpuinfo->vm_info.vm_stack_vaddr) {
    offset = vaddr - cpuinfo->vm_info.vm_bss_vaddr;
    paddr = cpuinfo->vm_info.vm_bss_paddr + offset;
    return paddr;
  } else if (vaddr >= cpuinfo->vm_info.vm_stack_vaddr &&
             vaddr <= cpuinfo->vm_info.vm_end_vaddr) {
    offset = vaddr - cpuinfo->vm_info.vm_stack_vaddr;
    paddr = cpuinfo->vm_info.vm_stack_paddr + offset;
    return paddr;
  } else {
    DEBUG ("Stack = %x\tStack End = %x\tBSS = %x\n", 0x9, cpuinfo->vm_info.vm_stack_vaddr, cpuinfo->vm_info.vm_end_vaddr, cpuinfo->vm_info.vm_bss_vaddr);
    panic ("virt2phys: address %x out of range on core %d!", vaddr, cpuinfo->cpuid);
  }
  return (phys_addr_t)0;
}


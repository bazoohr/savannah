#include <lib_mem.h>
#include <printf.h>
phys_addr_t
virt2phys (struct cpu_info *cpuinfo, virt_addr_t vaddr)
{
  phys_addr_t paddr;
  size_t offset;

#if 0
  printf ("vm_start = %x vm_end = %x\n", cpuinfo->vm_info.vm_start_vaddr,
      cpuinfo->vm_info.vm_end_vaddr);
  printf ("vm_start = %x vm_end = %x\n", cpuinfo->vm_info.vm_start_paddr,
      cpuinfo->vm_info.vm_end_paddr);
  printf ("vaddr = %x\n", vaddr);
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
             vaddr < cpuinfo->vm_info.vm_end_vaddr) {
    offset = vaddr - cpuinfo->vm_info.vm_stack_vaddr;
    paddr = cpuinfo->vm_info.vm_stack_paddr + offset;
    return paddr;
  } else {
    printf ("%s: Virt2Phys: address out of range!", __FILE__);
    for (;;);
  }
}


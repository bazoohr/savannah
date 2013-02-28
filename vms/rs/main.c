#include <cdef.h>
#include <types.h>
#include <debug.h>
#include <const.h>
#include <asmfunc.h>
#include <cpu.h>
#include <string.h>
#include <ipc.h>
#include <fs.h>
#include <pm.h>

struct cpu_info * __inline
get_cpu_info (const cpuid_t cpuid)
{
  phys_addr_t cpuinfo_base_paddr;

  if (cpuid > MAX_CPUS) {
    return NULL;
  }

  /*
   * TODO:
   *     Create a macro which hold the maximum size of cpu_info structure, and
   *     use it here (AND EVERYWHERE)  instead of _4KB_
   */
  cpuinfo_base_paddr = ((phys_addr_t)cpuinfo - (/*PM's cpuid */cpuinfo->cpuid * _4KB_));

  /* TODO:
   *     create a function and intitialize if the cpu_info structure is
   *     greater than _4KB_
   */
  /* TODO:
   *     One test to be done, is to allocate more than one page for one cpuuinfo
   *     structure, and solve the mess created afterward.
   */
  return (struct cpu_info *)(cpuinfo_base_paddr + cpuid * _4KB_);
}
void
vm_main (void)
{
  int i;
  for (i = 1; i < cpuinfo->ncpus; i++) {
    msg_reply (RS, i, 1, NULL, 0);
  }
  while (1) {__asm__ __volatile__ ("cli;pause;\n\t");}
}

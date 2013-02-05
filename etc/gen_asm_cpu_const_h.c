#include <stdio.h>
#include <cpu.h>
#include <cdef.h>

int main (void)
{
  printf ("/* Auto generated file. Don't edit! */\n");
  printf ("#ifndef __ASM_CPU_CONST_H__\n");
  printf ("#define __ASM_CPU_CONST_H__\n\n");

  printf ("#define CPU_LAPIC_ID %10ld\n", offsetof (struct cpu_info, lapic_id));
  printf ("#define CPU_CPUID %13ld\n", offsetof (struct cpu_info, cpuid));
  printf ("#define CPU_BOOTED %12ld\n", offsetof (struct cpu_info, booted));
  printf ("#define CPU_STACK %13ld\n", offsetof (struct cpu_info, vmm_info.vmm_stack_vaddr));
  printf ("#define CPU_CR3 %16ld\n", offsetof (struct cpu_info, vmm_info.vmm_page_tables));
  printf ("#define CPU_GDT %16ld\n",  offsetof (struct cpu_info, gdt));

  printf ("#endif\n");

  return 0;
}

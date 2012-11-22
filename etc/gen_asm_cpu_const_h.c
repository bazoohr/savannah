#include <stdio.h>
#include <cpu.h>
#include <cdef.h>

int main (void)
{
  printf ("/* Auto generated file. Don't edit! */\n");
  printf ("#ifndef __ASM_CPU_CONST_H__\n");
  printf ("#define __ASM_CPU_CONST_H__\n\n");

  printf ("#define CPU_LAPIC_ID %10ld\n", offsetof (struct cpu, lapic_id));
  printf ("#define CPU_CPUID %13ld\n", offsetof (struct cpu, cpuid));
  printf ("#define CPU_BOOTED %12ld\n", offsetof (struct cpu, booted));
  printf ("#define CPU_STACK %13ld\n", offsetof (struct cpu, stack));
  printf ("#define CPU_CR3 %13ld\n", offsetof (struct cpu, page_tables));
  printf ("#define CPU_GDT %15ld\n",  offsetof (struct cpu, gdt));

  printf ("#endif\n");

  return 0;
}
 

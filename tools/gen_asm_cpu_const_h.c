#include <stdio.h>
#include <cpu.h>
#include <cdef.h>

int main (void)
{
  printf ("/* Auto generated file. Don't edit! */\n");
  printf ("#ifndef __ASM_CPU_CONST_H__\n");
  printf ("#define __ASM_CPU_CONST_H__\n\n");

  printf ("#define CPU_REGS_RAX %10ld\n", offsetof (struct regs, rax));
  printf ("#define CPU_REGS_RBX %10ld\n", offsetof (struct regs, rbx));
  printf ("#define CPU_REGS_RCX %10ld\n", offsetof (struct regs, rcx));
  printf ("#define CPU_REGS_RDX %10ld\n", offsetof (struct regs, rdx));
  printf ("#define CPU_REGS_RSI %10ld\n", offsetof (struct regs, rsi));
  printf ("#define CPU_REGS_RDI %10ld\n", offsetof (struct regs, rdi));
  printf ("#define CPU_REGS_RSP %10ld\n", offsetof (struct regs, rsp));
  printf ("#define CPU_REGS_RIP %10ld\n", offsetof (struct regs, rip));
  printf ("#define CPU_REGS_RBP %10ld\n", offsetof (struct regs, rbp));
  printf ("#define CPU_REGS_CS  %10ld\n", offsetof (struct regs, cs));
  printf ("#define CPU_REGS_DS  %10ld\n", offsetof (struct regs, ds));
  printf ("#define CPU_REGS_ES  %10ld\n", offsetof (struct regs, es));
  printf ("#define CPU_REGS_FS  %10ld\n", offsetof (struct regs, fs));
  printf ("#define CPU_REGS_GS  %10ld\n", offsetof (struct regs, gs));
  printf ("#define CPU_REGS_SS  %10ld\n", offsetof (struct regs, ss));
  printf ("#define CPU_REGS_RFLAGS %7ld\n", offsetof (struct regs, rflags));
  printf ("#define CPU_REGS_R8 %11ld\n", offsetof (struct regs, r8));
  printf ("#define CPU_REGS_R9 %11ld\n", offsetof (struct regs, r9));
  printf ("#define CPU_REGS_R10 %10ld\n", offsetof (struct regs, r10));
  printf ("#define CPU_REGS_R11 %10ld\n", offsetof (struct regs, r11));
  printf ("#define CPU_REGS_R12 %10ld\n", offsetof (struct regs, r12));
  printf ("#define CPU_REGS_R13 %10ld\n", offsetof (struct regs, r13));
  printf ("#define CPU_REGS_R14 %10ld\n", offsetof (struct regs, r14));
  printf ("#define CPU_REGS_R15 %10ld\n", offsetof (struct regs, r15));
  printf ("#define CPU_REGS_CR0 %10ld\n", offsetof (struct regs, cr0));
  printf ("#define CPU_REGS_CR3 %10ld\n", offsetof (struct regs, cr3));
  printf ("#define CPU_REGS_CR4 %10ld\n", offsetof (struct regs, cr4));

  printf ("#define CPU_STRUCT_SIZE %9ld\n", offsetof (struct regs, cr4));

  printf ("#define CPU_LAPIC_ID %10ld\n", offsetof (struct cpu_info, lapic_id));
  printf ("#define CPU_CPUID %13ld\n", offsetof (struct cpu_info, cpuid));
  printf ("#define CPU_STACK %13ld\n", offsetof (struct cpu_info, vmm_info.vmm_regs.rsp));
  printf ("#define CPU_CR3 %16ld\n", offsetof (struct cpu_info, vmm_info.vmm_page_tables));

  printf ("#endif\n");

  return 0;
}

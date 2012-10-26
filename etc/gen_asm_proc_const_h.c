#include <stdio.h>
#include <proc.h>
#include <cdef.h>

int main (void)
{
  printf ("/* Auto generated file. Don't edit! */\n");
  printf ("#ifndef __ASM_PROCREGS_H__\n");
  printf ("#define __ASM_PROCREGS_H__\n\n");

  printf ("#define PR_RDI %10ld\n", offsetof (struct proc, p_registers.rdi));
  printf ("#define PR_RSI %10ld\n", offsetof (struct proc, p_registers.rsi));
  printf ("#define PR_RDX %10ld\n", offsetof (struct proc, p_registers.rdx));
  printf ("#define PR_RCX %10ld\n", offsetof (struct proc, p_registers.rcx));
  printf ("#define PR_R8 %11ld\n",  offsetof (struct proc, p_registers.r8));
  printf ("#define PR_R9 %11ld\n",  offsetof (struct proc, p_registers.r9));
  printf ("#define PR_RAX %10ld\n", offsetof (struct proc, p_registers.rax));
  printf ("#define PR_RBX %10ld\n", offsetof (struct proc, p_registers.rbx));
  printf ("#define PR_R10 %10ld\n", offsetof (struct proc, p_registers.r10));
  printf ("#define PR_R11 %10ld\n", offsetof (struct proc, p_registers.r11));
  printf ("#define PR_R12 %10ld\n", offsetof (struct proc, p_registers.r12));
  printf ("#define PR_R13 %10ld\n", offsetof (struct proc, p_registers.r13));
  printf ("#define PR_R14 %10ld\n", offsetof (struct proc, p_registers.r14));
  printf ("#define PR_R15 %10ld\n", offsetof (struct proc, p_registers.r15));

  printf ("#define PR_RFLAGS %7ld\n", offsetof (struct proc, p_registers.rflags));

  printf ("#define PR_RBP %10ld\n", offsetof (struct proc, p_registers.rbp));
  printf ("#define PR_RSP %10ld\n",  offsetof (struct proc, p_registers.rsp));

  printf ("#define PR_RIP %10ld\n", offsetof (struct proc, p_registers.rip));
  printf ("#define PR_CS %11ld\n",  offsetof (struct proc, p_registers.cs));

  printf ("#define PR_DS %11ld\n", offsetof (struct proc, p_registers.ds));
  printf ("#define PR_ES %11ld\n", offsetof (struct proc, p_registers.es));
  printf ("#define PR_FS %11ld\n", offsetof (struct proc, p_registers.fs));
  printf ("#define PR_GS %11ld\n", offsetof (struct proc, p_registers.gs));
  printf ("#define PR_SS %11ld\n", offsetof (struct proc, p_registers.ss));

  printf ("#define PR_CR3 %10ld\n\n", offsetof (struct proc, p_registers.cr3));

  printf ("#define PR_LDT_DESC %5ld\n", offsetof (struct proc, p_ldt_desc));
  printf ("#define PR_TSS_DESC %5ld\n", offsetof (struct proc, p_tss_desc));
  printf ("#define PR_REMAINED_QUANTUM  %6ld\n\n", offsetof (struct proc, p_remained_quantum));
  printf ("#define PR_MAX_QUANTUM  %6ld\n\n", offsetof (struct proc, p_max_quantum));
  printf ("#define PR_PID  %6ld\n\n", offsetof (struct proc, p_pid));

  printf ("#define PR_SIZE (PR_SS)\n\n");

  printf ("#endif\n");

  return 0;
}

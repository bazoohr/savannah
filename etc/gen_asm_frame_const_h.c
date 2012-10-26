#include <stdio.h>
#include <frame.h>
#include <cdef.h>

int main (void)
{
  printf ("/* Auto generated file. Don't edit! */\n");
  printf ("#ifndef __ASMFRAME_H__\n");
  printf ("#define __ASMFRAME_H__\n\n");

  printf ("#define TF_RDI %10ld\n", offsetof (struct trapframe, tf_rdi));
  printf ("#define TF_RSI %10ld\n", offsetof (struct trapframe, tf_rsi));
  printf ("#define TF_RDX %10ld\n", offsetof (struct trapframe, tf_rdx));
  printf ("#define TF_RCX %10ld\n", offsetof (struct trapframe, tf_rcx));
  printf ("#define TF_R8 %11ld\n",  offsetof (struct trapframe, tf_r8));
  printf ("#define TF_R9 %11ld\n",  offsetof (struct trapframe, tf_r9));
  printf ("#define TF_RAX %10ld\n", offsetof (struct trapframe, tf_rax));
  printf ("#define TF_RBX %10ld\n", offsetof (struct trapframe, tf_rbx));
  printf ("#define TF_RBP %10ld\n", offsetof (struct trapframe, tf_rbp));
  printf ("#define TF_R10 %10ld\n", offsetof (struct trapframe, tf_r10));
  printf ("#define TF_R11 %10ld\n", offsetof (struct trapframe, tf_r11));
  printf ("#define TF_R12 %10ld\n", offsetof (struct trapframe, tf_r12));
  printf ("#define TF_R13 %10ld\n", offsetof (struct trapframe, tf_r13));
  printf ("#define TF_R14 %10ld\n", offsetof (struct trapframe, tf_r14));
  printf ("#define TF_R15 %10ld\n", offsetof (struct trapframe, tf_r15));
  printf ("#define TF_TRAPNO %7ld\n",  offsetof (struct trapframe, tf_trapno));
  printf ("#define TF_FS %11ld\n",  offsetof (struct trapframe, tf_fs));
  printf ("#define TF_GS %11ld\n",  offsetof (struct trapframe, tf_gs));
  printf ("#define TF_DS %11ld\n",  offsetof (struct trapframe, tf_ds));
  printf ("#define TF_ES %11ld\n",  offsetof (struct trapframe, tf_es));

  printf ("#define TF_ERR %10ld\n", offsetof (struct trapframe, tf_err));
  printf ("#define TF_RIP %10ld\n", offsetof (struct trapframe, tf_rip));
  printf ("#define TF_CS %11ld\n",  offsetof (struct trapframe, tf_cs));
  printf ("#define TF_RFLAGS %7ld\n", offsetof (struct trapframe, tf_rflags));
  printf ("#define TF_RSP %10ld\n",  offsetof (struct trapframe, tf_rsp));
  printf ("#define TF_SS %11ld\n\n", offsetof (struct trapframe, tf_ss));

  printf ("#define TF_SIZE (TF_SS)\n\n");

  printf ("#endif\n");

  return 0;
}
 

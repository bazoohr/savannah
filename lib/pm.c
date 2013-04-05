#include <pm.h>
#include <ipc.h>
#include <string.h>
#include <lib_mem.h>
#include <debug.h>
#include <fs.h>
#include <misc.h>
#include <vuos/vuos.h>
/* ========================================================= */
int
empty (void)
{
  int result;

  msg_send (PM, 999, NULL, 0);
  msg_receive (PM);

  memcpy (&result, &cpuinfo->msg_input[PM].data, sizeof (int));

  return result;
}
/* ========================================================= */
int
fork_internal (virt_addr_t register_array_vaddr)
{
  int result;
  struct fork_ipc fork_args;

  fork_args.cpuinfo_vaddr = (virt_addr_t)&cpuinfo;
  fork_args.register_array_paddr = virt2phys (cpuinfo, register_array_vaddr);

  msg_send (PM, FORK_IPC, &fork_args, sizeof (struct fork_ipc));
  msg_receive (PM);

  memcpy (&result, &cpuinfo->msg_input[PM].data, sizeof (int));

  return result;
}
/* ========================================================= */
void
exec (char *path, char **argv)
{
  struct exec_ipc exec_args;
  size_t len;
  /*
   * XXX:
   *    Why static volatile???????
   *
   *    Actually we don't need neither static nor volatile normally. But
   *    Clang version 3.0 (Maybe other older version, we don't know)
   *    suffer from a bug that cause some stupid optimizations. For example
   *    in this case, despite the fact that we are using variable "registers"
   *    in this function, and pass its pointer to another function, it assums
   *    this variable as "unused!!!", and performs its own stupid optimizations
   *    on that.
   *    The good news is that this problem is already fixed in version 3.2.0.
   *    Therefore we use "static volatile" to cheat Clang! This way it can not
   *    make any wrong!! assumptions about this variable.
   *
   *    GCC works just fine in this case.
   */
  volatile static struct regs registers;

  len = strlen(path) > MAX_PATH ? MAX_PATH : strlen(path);

  strncpy(exec_args.path, path, len);
  exec_args.path[len] = '\0';

  exec_args.argv = argv != NULL ? (char **)virt2phys (cpuinfo, (virt_addr_t)argv) : NULL;

  /* Save the content of %rdi into RDI_IDX in cpuinfo */
  __asm__ __volatile__ (
      "movq %%rdi, %c[VM_REGS_RDI_IDX](%0)\n\t"
      "movq %%rax, %c[VM_REGS_RAX_IDX](%0)\n\t"
      :
      :"S"(&registers),
      [VM_REGS_RDI_IDX]"i"(offsetof (struct regs, rdi)),
      [VM_REGS_RAX_IDX]"i"(offsetof (struct regs, rax))
  );
  /* Save the content of cpuinfo in %rdi */
  /*
   * TODO:
   *     We most probably in future need to save some other registers
   *     including all segment registers, cr3, cr4, ... so that if exec
   *     fails we correctly restore the state of current executing VM.
   *
   *     At the moment, we are not going to cover those situations.
   */
  __asm__ __volatile__ (
      "pushfq;popq %%rax;movq %%rax, %c[VM_REGS_RFLAGS_IDX](%0)\n\t"
      "movq $exec_resume, %c[VM_REGS_RIP_IDX](%0)\n\t"
      "movq %%rsp, %c[VM_REGS_RSP_IDX](%0)\n\t"
      "movq %%rbp, %c[VM_REGS_RBP_IDX](%0)\n\t"
      "movq %%rbx, %c[VM_REGS_RBX_IDX](%0)\n\t"
      "movq %%rcx, %c[VM_REGS_RCX_IDX](%0)\n\t"
      "movq %%rdx, %c[VM_REGS_RDX_IDX](%0)\n\t"
      "movq %%rsi, %c[VM_REGS_RSI_IDX](%0)\n\t"
      "movq %%r8, %c[VM_REGS_R8_IDX](%0)\n\t"
      "movq %%r9, %c[VM_REGS_R9_IDX](%0)\n\t"
      "movq %%r10, %c[VM_REGS_R10_IDX](%0)\n\t"
      "movq %%r11, %c[VM_REGS_R11_IDX](%0)\n\t"
      "movq %%r12, %c[VM_REGS_R12_IDX](%0)\n\t"
      "movq %%r13, %c[VM_REGS_R13_IDX](%0)\n\t"
      "movq %%r14, %c[VM_REGS_R14_IDX](%0)\n\t"
      "movq %%r15, %c[VM_REGS_R15_IDX](%0)\n\t"
      :
      :
      "D"(&registers),
      [VM_REGS_RFLAGS_IDX]"i"(offsetof (struct regs, rflags)),
      [VM_REGS_RIP_IDX]"i"(offsetof (struct regs, rip)),
      [VM_REGS_RSP_IDX]"i"(offsetof (struct regs, rsp)),
      [VM_REGS_RBP_IDX]"i"(offsetof (struct regs, rbp)),
      [VM_REGS_RBX_IDX]"i"(offsetof (struct regs, rbx)),
      [VM_REGS_RCX_IDX]"i"(offsetof (struct regs, rcx)),
      [VM_REGS_RDX_IDX]"i"(offsetof (struct regs, rdx)),
      [VM_REGS_RSI_IDX]"i"(offsetof (struct regs, rsi)),
      [VM_REGS_R8_IDX]"i"(offsetof (struct regs, r8)),
      [VM_REGS_R9_IDX]"i"(offsetof (struct regs, r9)),
      [VM_REGS_R10_IDX]"i"(offsetof (struct regs, r10)),
      [VM_REGS_R11_IDX]"i"(offsetof (struct regs, r11)),
      [VM_REGS_R12_IDX]"i"(offsetof (struct regs, r12)),
      [VM_REGS_R13_IDX]"i"(offsetof (struct regs, r13)),
      [VM_REGS_R14_IDX]"i"(offsetof (struct regs, r14)),
      [VM_REGS_R15_IDX]"i"(offsetof (struct regs, r15))
  );

  exec_args.registers = virt2phys(cpuinfo, (virt_addr_t)&registers);

  msg_send (PM, EXEC_IPC, &exec_args, sizeof (struct exec_ipc));
  __asm__ __volatile__ (
      "vmcall\n\t"
      "exec_resume:\n\t"
  );
}
/* ========================================================= */
int
waitpid (int pid, int *status_buf, int options)
{
  struct waitpid_ipc waitpid_args;
  struct waitpid_reply *reply;

  waitpid_args.wait_for = pid;

  msg_send (PM, WAITPID_IPC, &waitpid_args, sizeof (struct waitpid_ipc));
  msg_receive (PM);

  reply = (struct waitpid_reply *)cpuinfo->msg_input[PM].data;

  *status_buf = reply->status;

  return reply->child_pid;
}

/* ========================================================= */
void
exit (int status)
{
  struct exit_ipc exit_args;

  exit_args.status = status;

  msg_send (PM, EXIT_IPC, &exit_args, sizeof (struct exit_ipc));
  __asm__ __volatile__ ("vmcall\n\t");
}
/* ========================================================= */

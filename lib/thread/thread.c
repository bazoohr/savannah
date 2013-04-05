#include <frame.h>
#include <thread.h>
#include <timer.h>
#include <lapic.h>
#include <interrupt.h>
#include <gdt.h>
#include <panic.h>
#include <vuos/vuos.h>
#include <debug.h>       /* TODO: Remove it after debugging */
#include <asmfunc.h>     /* TODO: Remove it after debugging */
/* ============================================ */
extern void timer_handler (void);
/* ============================================ */
static thread_t *default_scheduler (void);
/* ============================================ */
static thread_t *(*scheduler)(void) = default_scheduler;
static thread_t main_thread;
thread_t *head;
thread_t *last;
thread_t *current;
/* ============================================ */
static thread_t *default_scheduler (void)
{
  return current = current->nxt ? current->nxt : head;
}
/* ============================================ */
void
thread_switch_handler (struct intr_stack_frame *current_regs)
{
  DEBUG ("This is tread_switch_handler", 0xE);
  static volatile thread_t *next;
  if (head == last) {
  DEBUG ("No thread defined", 0xB);
    lapic_eoi ();  /* XXX: Should we move it to timer_isr.S??? */
    return;
  }
  /* Regsiters are saved on stack, so we keep a pointer to
   * the beginning of frame pointer */
  current->regs = current_regs;
  /* Default, a basic round-robin algorithm */
  next = scheduler ();
  current = (thread_t*)next;
  /*
   * TODO:
   *     Move the rest of this routine to another function
   *     called switchto(current, next) so as to allow users
   *     to define their own schedulers which just feeds current
   *     and next thread to switch to function.
   */
  __asm__ __volatile__ (
      "movq %c[THREAD_REGS_RFLAGS_IDX](%0), %%rax\n\t"
      "pushq %%rax; popfq;\n\t"
      "movq $0xFEE000B0, %%rax\n\t"
      "movq %c[THREAD_REGS_RCX_IDX](%0), %%rcx\n\t"
      "movq %c[THREAD_REGS_RDX_IDX](%0), %%rdx\n\t"
      "movq %c[THREAD_REGS_RSI_IDX](%0), %%rsi\n\t"
      "movq %c[THREAD_REGS_R8_IDX](%0), %%r8\n\t"
      "movq %c[THREAD_REGS_R9_IDX](%0), %%r9\n\t"
      "movq %c[THREAD_REGS_R10_IDX](%0), %%r10\n\t"
      "movq %c[THREAD_REGS_R11_IDX](%0), %%r11\n\t"
      "movq %c[THREAD_REGS_R12_IDX](%0), %%r12\n\t"
      "movq %c[THREAD_REGS_R13_IDX](%0), %%r13\n\t"
      "movq %c[THREAD_REGS_R14_IDX](%0), %%r14\n\t"
      "movq %c[THREAD_REGS_R15_IDX](%0), %%r15\n\t"
      "movq %c[THREAD_REGS_RSP_IDX](%0), %%rsp\n\t"
      "movq %c[THREAD_REGS_RBP_IDX](%0), %%rbp\n\t"
      "movq %c[THREAD_REGS_RIP_IDX](%0), %%rbx\n\t"
      "pushq %%rbx\n\t"  /* Overwrites SS in interrupt stack frame */
      "movl $0, (%%rax)\n\t"  /* EOI + short delay in folowing lines */
      "movq %c[THREAD_REGS_RAX_IDX](%0), %%rax\n\t"
      "movq %c[THREAD_REGS_RBX_IDX](%0), %%rbx\n\t"
      "movq %c[THREAD_REGS_RDI_IDX](%0), %%rdi\n\t"
      "retq"
      :
      :
      "D"(next->regs),
      [THREAD_REGS_RFLAGS_IDX]"i"(offsetof (struct intr_stack_frame, tf_rflags)),
      [THREAD_REGS_RCX_IDX]"i"(offsetof (struct intr_stack_frame, tf_rcx)),
      [THREAD_REGS_RDX_IDX]"i"(offsetof (struct intr_stack_frame, tf_rdx)),
      [THREAD_REGS_RSI_IDX]"i"(offsetof (struct intr_stack_frame, tf_rsi)),
      [THREAD_REGS_R8_IDX]"i"(offsetof (struct intr_stack_frame, tf_r8)),
      [THREAD_REGS_R9_IDX]"i"(offsetof (struct intr_stack_frame, tf_r9)),
      [THREAD_REGS_R10_IDX]"i"(offsetof (struct intr_stack_frame, tf_r10)),
      [THREAD_REGS_R11_IDX]"i"(offsetof (struct intr_stack_frame, tf_r11)),
      [THREAD_REGS_R12_IDX]"i"(offsetof (struct intr_stack_frame, tf_r12)),
      [THREAD_REGS_R13_IDX]"i"(offsetof (struct intr_stack_frame, tf_r13)),
      [THREAD_REGS_R14_IDX]"i"(offsetof (struct intr_stack_frame, tf_r14)),
      [THREAD_REGS_R15_IDX]"i"(offsetof (struct intr_stack_frame, tf_r15)),
      [THREAD_REGS_RSP_IDX]"i"(offsetof (struct intr_stack_frame, tf_rsp)),
      [THREAD_REGS_RBP_IDX]"i"(offsetof (struct intr_stack_frame, tf_rbp)),
      [THREAD_REGS_RIP_IDX]"i"(offsetof (struct intr_stack_frame, tf_rip)),
      [THREAD_REGS_RAX_IDX]"i"(offsetof (struct intr_stack_frame, tf_rax)),
      [THREAD_REGS_RBX_IDX]"i"(offsetof (struct intr_stack_frame, tf_rbx)),
      [THREAD_REGS_RDI_IDX]"i"(offsetof (struct intr_stack_frame, tf_rdi))
  );
}
/* ============================================ */
void
thread_create (thread_t *thread,
               void (*routine)(void*),
               void *stack,
               size_t stack_size)
{
  register_t rsp;
  if (unlikely (head ==NULL || last == NULL)) {
    panic ("thread linrary is not initialized yet");
  }
  cli ();

  rsp = (virt_addr_t)stack + stack_size;

  thread->regs = (struct intr_stack_frame*)(rsp - sizeof (struct intr_stack_frame));
  thread->regs->tf_rip = (virt_addr_t)routine;
  thread->regs->tf_rsp = (virt_addr_t)stack + stack_size;
  thread->regs->tf_rbp = thread->regs->tf_rsp;
  thread->regs->tf_rflags = 0x2 | RFLAGS_IF;  /* Interrupts are enabled */
  thread->nxt = NULL;
  last->nxt = thread;
  last = thread;

  sti ();

}
/* ============================================ */
void
thread_set_scheduler (thread_t *(*new_scheduler)(void))
{
  if (unlikely (head ==NULL || last == NULL)) {
    panic ("thread linrary is not initialized yet");
  }
  cli ();
  scheduler = new_scheduler;
  sti ();
}
/* ============================================ */
void
thread_set_timer_freq (uint64_t ms)
{
  if (unlikely (head ==NULL || last == NULL)) {
    panic ("thread linrary is not initialized yet");
  }
  cli ();
  timer_off ();
  init_timer ();
  timer_on (ms);
  sti ();
}
/* ============================================ */
void
thread_init (void)
{
  main_thread.nxt = NULL;

  current = &main_thread;
  head    = &main_thread;
  last    = &main_thread;

  create_default_gdt ();
  interrupt_init ();
  init_timer ();
  add_irq (32, &timer_handler);
  sti ();
  timer_on (100);
}

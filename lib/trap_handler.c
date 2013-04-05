#include <trap.h>
#include <debug.h>
#include <types.h>
#include <asmfunc.h>

void trap_handler (struct intr_stack_frame *tf)
{
  if (tf->tf_trapno == IDT_PF) {
    phys_addr_t addr = rcr2 ();
    DEBUG ("page fault addr = %x\n", 0x4, addr);
  } else {
    DEBUG ("trap number %d\n", 0x4, tf->tf_trapno);
    DEBUG ("cs %x\n", 0x4, tf->tf_cs);
    DEBUG ("rbp %x\n", 0x4, tf->tf_rbp);
    DEBUG ("rsp %x\n", 0x4, tf->tf_rsp);
    DEBUG ("rip %x\n", 0x4, tf->tf_rip);
    DEBUG ("trap_no %x\n", 0x4, tf->tf_trapno);
    DEBUG ("error %x\n", 0x4, tf->tf_err);
    halt ();
  }
}


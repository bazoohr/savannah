#include <trap.h>
#include <printk.h>
#include <types.h>
#include <asmfunc.h>

void trap_handler (struct trapframe *tf)
{
  if (tf->tf_trapno == IDT_PF) {
    phys_addr_t addr = rcr2 ();
    //printk ("page fault addr = %x process id %d\n", addr, current->p_pid);
    printk ("page fault addr = %x\n", addr);
  }

  printk ("trap number %d\n", tf->tf_trapno);
  printk ("cs %x\n", tf->tf_cs);
  printk ("rbp %x\n", tf->tf_rbp);
  printk ("rsp %x\n", tf->tf_rsp);
  printk ("rip %x\n", tf->tf_rip);
  printk ("trap_no %x\n", tf->tf_trapno);
  printk ("error %x\n", tf->tf_err);
  halt ();
}


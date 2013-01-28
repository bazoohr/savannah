#include <trap.h>
#include <printk.h>
#include <types.h>
#include <asmfunc.h>

static void syscall_handler (struct trapframe *tf)
{
#if 0
  switch (tf->tf_rax) {
    case SYS_PUTS:
      kputs ((char*)tf->tf_rbx, tf->tf_rcx);
      break;
    case SYS_PUTC:
      kputc ((char)tf->tf_rbx, tf->tf_rcx);
      break;
    case SYS_GOTOXY:
      change_cursor_pos (tf->tf_rbx, tf->tf_rcx);
      break;
    case SYS_FORK:
      load_cr3 (kernel_pml4);
      do_fork (tf);
      load_cr3 (current->p_pml4);
      break;
    case SYS_CLONE:
      load_cr3 (kernel_pml4);
      do_clone (tf);
      load_cr3 (current->p_pml4);
      break;
    case SYS_WHEREXY:
      get_cursor_pos (tf->tf_rbx, tf->tf_rcx);
      break;
    case SYS_EXIT:
      load_cr3 (kernel_pml4);
      do_exit (tf->tf_rbx);
      load_cr3 (current->p_pml4);
      break;
    case SYS_WAITPID:
      tf->tf_rax = do_waitpid (tf->tf_rbx, (int*)tf->tf_rcx);
      break;
    case SYS_READ:
      tf->tf_rax = do_read (tf->tf_rbx, (char*)tf->tf_rcx, tf->tf_rdx);
      break;
    case SYS_UNCLONE:
      load_cr3 (kernel_pml4);
      do_unclone ((void *)tf->tf_rbx);
      load_cr3 (current->p_pml4);
      break;
    case SYS_SBRK:
      load_cr3 (kernel_pml4);
      tf->tf_rax = (virt_addr_t)do_sbrk ((size_t)tf->tf_rbx);
      load_cr3 (current->p_pml4);
      break;
    case SYS_BRK:
      load_cr3 (kernel_pml4);
      tf->tf_rax = (virt_addr_t)do_brk ((size_t)tf->tf_rbx);
      load_cr3 (current->p_pml4);
      break;

    default:
      cprintk ("SYSCALL: ERROR:", 0x4);
      cprintk ("Unimplemented system call invoked %d", 0xE, tf->tf_rax);
      break;
  }
  schedule ();
#endif
}
void trap_handler (struct trapframe *tf)
{
  if (tf->tf_trapno == IDT_PF) {
    phys_addr_t addr = rcr2 ();
    //printk ("page fault addr = %x process id %d\n", addr, current->p_pid);
    printk ("page fault addr = %x\n", addr);
  }

  if (tf->tf_trapno == TRAP_SYSCALL) {
    syscall_handler (tf);
    return;
  } else {
    printk ("trap number %d\n", tf->tf_trapno);
    printk ("cs %x\n", tf->tf_cs);
    printk ("rbp %x\n", tf->tf_rbp);
    printk ("rsp %x\n", tf->tf_rsp);
    printk ("rip %x\n", tf->tf_rip);
    printk ("trap_no %x\n", tf->tf_trapno);
    printk ("error %x\n", tf->tf_err);
    halt ();
  }
}


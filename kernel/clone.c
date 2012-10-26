/* ============================================ *
 * clone.c                                       *
 *                                              *
 * Hamid R. Bazoobandi                          *
 * August 6 2012 Amsterdam                      *
 * ============================================ */
#include <proc.h>
#include <mmap.h>
#include <frame.h>
#include <string.h>
#include <page.h>
#include <memory.h>
#include <schedule.h>
#include <printk.h>
#include <panic.h>
#include <lock.h>
#include <global.h>
#include <asmfunc.h>
#include <macro.h>

/*
 * FIXME:
 *     In case of failure this function should return -1
 *     to parent, and no thread should be created!
 */
void
do_clone (struct trapframe *tf)
{
  struct proc *thread;
  struct proc *parent = current;
  void *(*thread_routine)(void *args) = (void *(*)(void*))tf->tf_rbx;
  void *args = (void *)tf->tf_rcx;
  virt_addr_t stack_vaddr = (virt_addr_t)tf->tf_rdx;
  size_t stack_size = (size_t)tf->tf_rdi;
  int lock;
  int i;

  if (stack_size < 4096) {
    cprintk ("%s: Too little stack!\n", 0x4, __func__);
    return;
  }
  thread = create_empty_proc ();

  thread->p_pml4 = parent->p_pml4;
  thread->p_registers.cr3 = parent->p_pml4;

  thread->p_pstack = virt2phys (stack_vaddr, parent);
  if (!thread->p_pstack) {
    cprintk ("ERROR: %s bad stack virtual addr", 0x4, __func__);
  }

  /* XXX:
   *    system stack would be different for parent and thread
   */
  thread->p_tss.tss_rsp0 = thread->p_pstack + 4096;/*ALIGN (thread->p_pstack + 1024, 16);*/
  thread->p_registers.rsp = stack_vaddr + 2*4096;/*ALIGN( stack_vaddr + 2 * 1024, 16);*/

  map_proc_pages (thread, thread->p_pstack, thread->p_pstack, 1, BSS_SEG);
  thread->p_registers.rip = (virt_addr_t)thread_routine;  /* thread routine */
  thread->p_registers.rdi = (virt_addr_t)args;  /* Argument of thread routine */

  thread->p_registers.rflags = (1<<9);

  thread->p_registers.cs = USR_CSEL;
  thread->p_registers.ds = USR_DSEL;
  thread->p_registers.es = USR_DSEL;
  thread->p_registers.fs = USR_DSEL;
  thread->p_registers.gs = USR_DSEL;
  thread->p_registers.ss = USR_DSEL;

  thread->p_remained_quantum = PROC_QUANTUM;
  thread->p_max_quantum = PROC_QUANTUM;

  thread->p_state = PROC_STATE_READY;

  thread->p_flags = PROC_FLAGS_ISTHREAD;

  thread->p_registers.rax = 0;   /* return value to thread */
  tf->tf_rax = thread->p_pid;    /* return value to parent */

  thread->p_type = 3;

  thread->p_ppid = parent->p_pid;

  for (i = 0; i < RS_MAX; i++) {
    thread->p_resources[i] = 0;
  }

  create_ldt (&thread->p_ldt, USR_PVL);

  create_ldt_descriptor (&thread->p_ldt, &thread->p_ldt_desc);
  create_tss_descriptor (&thread->p_tss, &thread->p_tss_desc);

  /* FIXME:
   *   We have to currentialize the list in a way that immediatly after
   *   kernel primary stuffs, idle is the first process which is scheduled
   */
  lock_region (&lock);

  thread->next = current->next;
  current->next = thread;
  thread->pre = current;

  unlock_region (&lock);
}

/*
 * TODO:
 *   If parent thread is terminated without waiting for terminated thread
 *   then we'd better remove it from the process list.
 *   In current version, it will remain as a zombie in the list for ever.
 */
void
do_unclone (void __unused *status)
{
  struct proc *parent;
  if (current->p_pid > 1) {
    parent = get_pcb (current->p_ppid);
  
    if (parent->p_flags & PROC_FLAGS_WCHLDPID ||
        parent->p_flags & PROC_FLAGS_WCHLDANY) {
      parent->p_state = PROC_STATE_READY;
    }
  } 
  /* XXX:
   *    After freeing all pages, there would be no stack to
   *    handle interrupt, so we can't receive any interrupt
   *    before giving cpu to another process!
   *    Interrupts will be automatically enabled after next
   *    taskswitch with loading next process's flags register.
   */
  cli ();
  unmap_proc_pages (current, current->p_pstack, 1);
  current->p_state = PROC_STATE_ZOMBIE;
  current->p_exit_status = *((int*)status);

  schedule ();
}

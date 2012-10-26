#include <proc.h>
#include <page.h>
#include <memory.h>
#include <printk.h>
#include <schedule.h>
#include <mmap.h>
#include <global.h>
#include <asmfunc.h>

/*
 * TODO:
 *   If parent process is terminated without waiting for terminated process
 *   then we'd better remove it from the process list.
 *   In current version it remains as zombie in the list for ever.
 */
void
do_exit (int status)
{
  struct proc *parent;
  /*
   * FIXME:
   *     If a thread executes exit, then we have to
   *     find all other threads belonging to that process
   *     and remove them, as well as the main process.
   *     
   *     Isn't this TOO MUCH for an educational OS????
   */
  if (current->p_pid > 1) {
    parent = get_pcb (current->p_ppid);
  
    /*
     * FIXME:
     *      We can not simply put parent in READY state
     *      without checking whether the terminated child
     *      is exactly what we were waiting for or not.
     *      THIS IS POTENTIALLY A SERIOUS BUG
     */
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
  current->p_state = PROC_STATE_ZOMBIE;
  current->p_exit_status = status;
  free_proc_pages (current);

  schedule ();
}

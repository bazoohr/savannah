#include <proc.h>
#include <printk.h>
#include <dev/pic.h>
#include <asmfunc.h>
#include <lock.h>
#include <global.h>

void
schedule (void)
{
  struct proc *start_point = NULL;
  struct proc *nxt  = NULL;
  int lock;

  lock_region (&lock);

  /*
   * if the time quantum of current process is exhausted or if its blocked, we're
   * going to pick another process. otherwise the same process keeps on its execution.
   */
  if (current->p_state != PROC_STATE_READY || current->p_remained_quantum <= 0) {
    /*
     * XXX:
     *    * idle is not in process list. If current process is IDLE, then we start
     *      searching for new process right from the begining of process list, which
     *    * If current process is not idle, then we start searching, right from the
     *      current position of list.
     *      is init process.
     *    * If no ready process is found, idle gets the CPU.
     */
    start_point = current->p_pid != IDLE_PID ? current : get_pcb (INIT_PID);
    nxt = current->p_pid != IDLE_PID ? current->next : get_pcb (INIT_PID);

    do {
      if (nxt->p_state == PROC_STATE_READY) {
        break;
      }
     
      nxt = nxt->next;
      /*
       * If we arrive to start_point, it means that we didn't find
       * any ready process in the whole list.
       */
      if (nxt == start_point) {
        nxt = NULL;
        break;
      }
    } while (1);

    /*
     * If no ready process found (this is what "nxt == NULL" means), then if current process
     * is still ready to continue its execution, we let it go, but if it is blocked, then we
     * have no ready process in the system to get the CPU, so we give the CPU to idle process.
     */
    if (nxt == NULL && current->p_pid != IDLE_PID && current->p_state != PROC_STATE_READY) {
      nxt = get_pcb (IDLE_PID);  /* next process would be idle */
    }
    if (nxt && nxt != current) {
      switchto (current, nxt);
    }
  }

  unlock_region (&lock);
}

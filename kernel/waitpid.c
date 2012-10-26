#include <proc.h>
#include <pid.h>
#include <schedule.h>
#include <printk.h>
#include <global.h>

/* XXX:
 *    Since this function does not work like normal routines (probably we will
 *    have a context switch inside this function), GCC or CLANG might do some
 *    stupid things in high optimization levels.
 *    Therefore, any further changes must be applies very carefully. Don't do anything
 *    with this function if you're not sure about what you are doing.
 */
pid_t
do_waitpid (const pid_t pid, int * const status)
{
  struct proc *child;
  struct proc *parent;

  if (pid > NPID) {
    cprintk ("returning pid > NPID!\n", 0xC);
    return -1;
  }

  child  = pid > 0 ? get_pcb (pid) : NULL;
  parent = current;

  if (child && child->p_ppid != parent->p_pid) {
    cprintk ("returning chi;d == NULL!\n", 0xC);
    return -1;
  }

  while (true) {
    if (child) { /* waitpid (pid, &status) */
      if (child->p_state == PROC_STATE_ZOMBIE) {
        if (status) {
          *status = child->p_exit_status;
        }

        child->p_state = PROC_STATE_READY;
        child->pre->next = child->next;
        child->next->pre = child->pre;

        free_pid (child->p_pid);

        parent->p_flags &= ~PROC_FLAGS_WCHLDPID;

        return child->p_pid;
      } else {
        parent->p_state = PROC_STATE_BLOCKED;
        parent->p_flags |= PROC_FLAGS_WCHLDPID;
      }
    } else {  /* waitpid (-1, &status) */
      struct proc *p = parent->next;

      while (p != current) {
        if (p->p_ppid == parent->p_pid) {
          if (p->p_state == PROC_STATE_ZOMBIE) {
            /* remove child process from process list */
            p->pre->next = p->next;
            p->next->pre = p->pre;

            if (status) {
              *status = p->p_exit_status;
            }

            free_pid (p->p_pid);

            parent->p_flags &= ~PROC_FLAGS_WCHLDANY;
            return p->p_pid;
          }
        }

        p = p->next;
        /* XXX:
         *   If a process with no child execute waitpid (-1, &status)
         *   it will get blocked for ever.
         */
      }

      parent->p_state = PROC_STATE_BLOCKED;
      parent->p_flags |= PROC_FLAGS_WCHLDANY;
    }

    schedule ();
  }
  return -1;
}

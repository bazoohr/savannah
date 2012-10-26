#include <resource.h>
#include <proc.h>
#include <panic.h>
#include <schedule.h>
#include <lock.h>
#include <printk.h>

extern struct proc *current;

static struct resource resource_table[RS_MAX];

void
resource_init (void)
{
  int i;

  for (i = 0; i < RS_MAX; i++) {
    struct resource *rs;
    
    rs = &resource_table[i];

    rs->rs_number = i;
    rs->rs_status = RS_FREE;

    qinit (&rs->rs_usr_queue);
    qinit (&rs->rs_sys_queue);
  }
}

void
resource_lock (int resource_no)
{
  struct resource *rs;
  int lock;

  if (resource_no < 0 || resource_no >= RS_MAX) {
    panic ("bad resource number!");
  }

  lock_region (&lock);
  rs = &resource_table[resource_no];

  if (rs->rs_status != RS_FREE) {
    if (!qpush (&rs->rs_usr_queue, current)){
      panic ("Failed to put process in resource queue!");
    }
    current->p_state = PROC_STATE_BLOCKED;

    unlock_region (&lock);

    schedule ();
  } else {
    rs->rs_status = RS_BUSY;
    unlock_region (&lock);
  }
  current->p_resources[resource_no]++;
}

void
resource_unlock (int resource_no)
{
  struct resource *rs;
  int lock;

  if (resource_no < 0 || resource_no >= RS_MAX) {
    panic ("bad resource number!\n");
  }

  lock_region (&lock);
  rs = &resource_table[resource_no];

  if (!qempty (&rs->rs_usr_queue)) {
    struct proc *p;

    p = (struct proc *)qpop (&rs->rs_usr_queue);
    p->p_state = PROC_STATE_READY;
    unlock_region (&lock);
    schedule ();
  } else {
    rs->rs_status = RS_FREE;
    unlock_region (&lock);
  }
}

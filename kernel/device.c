#include <proc.h>
#include <cdef.h>
#include <panic.h>
#include <schedule.h>
#include <device.h>
#include <printk.h>
#include <string.h>
#include <lock.h>

extern struct proc *current;

static union device devs[NDEV];

static bool __inline
is_indev (int devno)
{
  /* 
   * Currently, only keyboard (id 0) is input device.
   */
  return devno == DEV_KBD ? true : false;
}

void 
device_register (int devno, union device *dev)
{
  if (devno >= NDEV) {
    panic ("invalid device id!\n");
  }
  devs[devno] = *dev;
}

/*
 * These functions are only used for input devices
 */
void
device_wait_for_data (int idevno)
{
  struct input_device *id;
  int lock;

  if (idevno >= NDEV) {
    panic ("invalid device id!\n");
  }

  if (!is_indev (idevno)) {
    panic ("Wait for data from an output device!");
  }

  id = &devs[idevno].idev;

  lock_region (&lock);
  id->id_waiting_proc = current;
  current->p_state = PROC_STATE_BLOCKED;
  unlock_region (&lock);
  schedule ();
}
int
device_read (int idevno, char *const buf)
{
  struct input_device *id;

  if (idevno >= NDEV) {
    panic ("invalid device id!\n");
  }

  if (!is_indev (idevno)) {
    panic ("Wait for data from an output device!");
  }

  id = &devs[idevno].idev;

  switch (idevno) {
    case DEV_KBD: {
      int key_code;
      
      key_code = id->id_read ();
      if (key_code < 0) {
        return -1;
      }

      memcpy (buf, &key_code, sizeof (int));

      return sizeof (int);
    }
    default:
      /* We should not get to this point */
      break;
  }
  return -1;
}

void
device_signal_data_ready (int idevno)
{
  struct proc *waiting_proc;
  struct input_device *id;
  int lock;

  if (!is_indev (idevno)) {
    panic ("device_signal_data_ready() called for non-input device!\n");
  }

  lock_region (&lock);
  id = &devs[idevno].idev;

  waiting_proc = id->id_waiting_proc;
  if (waiting_proc) {
    if (waiting_proc->p_state == PROC_STATE_BLOCKED) {
      waiting_proc->p_state = PROC_STATE_READY;
      id->id_waiting_proc = NULL;
    } else {
      /* Something fishy is going on then */
    }
  }

  unlock_region (&lock);
  schedule ();
}

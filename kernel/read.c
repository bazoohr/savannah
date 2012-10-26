#include <proc.h>
#include <device.h>
#include <panic.h>
#include <resource.h>
#include <printk.h>
extern struct proc *current;
int 
do_read (int idev, char *buf, size_t size)
{
  int i = 0;
  int nbytes;
  
  resource_lock (RS_KEYBRD);

  while (i < size) {
    device_wait_for_data (idev);
    
    nbytes = device_read (idev, buf);

    if (nbytes < 0) {
      panic ("Failed to read from device!");
    }

    buf += nbytes;
    i += nbytes;
  }
  resource_unlock (RS_KEYBRD);
  return i;
}




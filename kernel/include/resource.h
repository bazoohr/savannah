#ifndef __RESOURCE_H__
#define __RESOURCE_H__


#define RS_SCREEN 0
#define RS_KEYBRD 1
#define RS_MAX    2

#include <queue.h>
#include <device.h>

struct resource {
  int rs_number;
  int rs_status;
#define RS_BUSY 0
#define RS_FREE 1
  queue_t rs_usr_queue;
  queue_t rs_sys_queue;
};

void resource_init (void);
void resource_lock (int resource_no);
void resource_unlock (int resource_no);

#endif /* __RESOURCE_H__ */

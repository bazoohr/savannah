#ifndef __IPC_H__
#define __IPC_H__

#include <cpuinfo.h>

void msg_send (const int to, const int data);
int msg_receive ();


#endif /* __IPC_H__ */

#ifndef __IPC_H__
#define __IPC_H__

#include <cpuinfo.h>

#define PM	0
#define FS	1

void msg_send (const int to, const int number, const void *data, int size);
struct message msg_receive (int from);


#endif /* __IPC_H__ */

#ifndef __IPC_H__
#define __IPC_H__

#include <cpuinfo.h>

#define PM	0
#define FS	1

void msg_send (const int to, const int number, const void *data, const int size);
void msg_receive ();
struct message *msg_check();
void msg_reply(const int to, const int number, const void *data, const int size);


#endif /* __IPC_H__ */

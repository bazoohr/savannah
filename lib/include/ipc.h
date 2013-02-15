#ifndef __IPC_H__
#define __IPC_H__

#include <cpuinfo.h>

#define ANY  (-1)

void msg_send (const int to, const int number, const void *data, const int size);
int msg_receive (int from);
struct message *msg_check();
void msg_reply(const int to, const int number, const void *data, const int size);


#endif /* __IPC_H__ */

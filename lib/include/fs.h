#ifndef __FS_H__
#define __FS_H__

#include <types.h>

#define O_RDONLY        00000000
#define O_WRONLY        00000001
#define O_RDWR          00000002

#define OPEN_IPC	1
#define READ_IPC	2
#define CLOSE_IPC       3
#define LOAD_IPC        4
#define PUTC_IPC        5
#define READ_ACK        6
#define WRITE_IPC       7
#define WRITE_ACK       8

#define TYPE_FILE 1
#define TYPE_CHAR 2

#define MAX_PATHNAME    32

#define MAX_FD (64)

// TODO Create just one structure for all the char devices
struct keyboard_read {
  cpuid_t from;
  void *channel;
  size_t count;
};

struct console_write {
  cpuid_t from;
  void *channel;
  size_t count;
};

struct open_ipc {
	char pathname[MAX_PATHNAME];
	int flags;
};

struct open_reply {
	int fd;
	void *channel;
};

struct read_ipc {
	int fd;
	char *buf;
	int count;
};

struct write_ipc {
	int fd;
	void *buf;
	int count;
};

struct read_reply {
	int from;
	int type;
	int count;
	void *channel;
};

struct write_reply {
	int from;
	int count;
};

struct close_ipc {
	int fd;
};

int open(const char *pathname, int flags);
int read(int fd, void *buf, int count);
int write(int fd, void *buf, int count);
int close(int fd);

#endif /* __FS_H__ */

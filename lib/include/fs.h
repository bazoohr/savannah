#ifndef __FS_H__
#define __FS_H__

#define O_RDONLY        00000000
#define O_WRONLY        00000001
#define O_RDWR          00000002

#define OPEN_IPC	1
#define READ_IPC	2
#define CLOSE_IPC       3
#define LOAD_IPC        4
#define PUTC_IPC        5

#define MAX_PATHNAME    32
#define MAX_BUFFER      32

struct open_ipc {
	char pathname[MAX_PATHNAME];
	int flags;
};

struct read_ipc {
	int fd;
	char buf[MAX_BUFFER]; // TODO Remove this and use a pointer once we have a PM/MM
	int count;
};

struct close_ipc {
	int fd;
};

int open(const char *pathname, int flags);
int read(int fd, void *buf, int count);
int close(int fd);

#endif /* __FS_H__ */

#include <fs.h>
#include <ipc.h>
#include <string.h>

int open(const char *pathname, int flags)
{
	struct open_ipc tmp;

	strcpy(tmp.pathname, pathname);
	tmp.flags = flags;

	msg_send(FS, OPEN_IPC, &tmp, sizeof(struct open_ipc));
	struct message m = msg_receive(FS);

	int r = (int)*m.data;

	return r;
}

int read(int fd, void *buf, int count)
{
	struct read_ipc tmp;

	tmp.fd = fd;
	tmp.count = count;

	msg_send(FS, READ_IPC, &tmp, sizeof(struct read_ipc));
	struct message m = msg_receive(FS);

	memcpy(&tmp, m.data, sizeof(struct read_ipc));

	strncpy(buf, tmp.buf, tmp.count);

	int r = tmp.count;

	return r;
}

int close(int fd)
{
	struct close_ipc tmp;

	tmp.fd = fd;

	msg_send(FS, CLOSE_IPC, &tmp, sizeof(struct close_ipc));
	struct message m = msg_receive(FS);

	int r = (int)*m.data;

	return r;
}

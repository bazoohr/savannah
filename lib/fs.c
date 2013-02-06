#include <fs.h>
#include <ipc.h>
#include <string.h>

int open(const char *pathname, int flags)
{
  struct open_ipc tmp;

  int len = strlen(pathname) > MAX_PATHNAME ? MAX_PATHNAME : strlen(pathname);

  strncpy(tmp.pathname, pathname, len);
  tmp.flags = flags;

  msg_send(FS, OPEN_IPC, &tmp, sizeof(struct open_ipc));
  msg_receive();

  int r;
  memcpy(&r, &cpuinfo->msg_input->data, sizeof(int));

  return r;
}

int read(int fd, void *buf, int count)
{
  struct read_ipc tmp;

  tmp.fd = fd;
  tmp.count = count > MAX_BUFFER ? MAX_BUFFER : count;

  msg_send(FS, READ_IPC, &tmp, sizeof(struct read_ipc));
  msg_receive();

  memcpy(&tmp, &cpuinfo->msg_input->data, sizeof(struct read_ipc));

  strncpy(buf, tmp.buf, tmp.count);

  return tmp.count;
}

int close(int fd)
{
  struct close_ipc tmp;

  tmp.fd = fd;

  msg_send(FS, CLOSE_IPC, &tmp, sizeof(struct close_ipc));
  msg_receive();

  int r;
  memcpy(&r, &cpuinfo->msg_input->data, sizeof(int));

  return r;
}

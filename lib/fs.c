#include <fs.h>
#include <ipc.h>
#include <string.h>
#include <config.h>

#include <printk.h>

int open(const char *pathname, int flags)
{
  struct open_ipc tmp;
  int r;
  size_t len;

  len = strlen(pathname) > MAX_PATHNAME ? MAX_PATHNAME : strlen(pathname);

  strncpy(tmp.pathname, pathname, len);
  tmp.pathname[len] = '\0';
  tmp.flags = flags;

  msg_send(FS, OPEN_IPC, &tmp, sizeof(struct open_ipc));
  msg_receive(FS);

  memcpy(&r, &cpuinfo->msg_input[FS].data, sizeof(int));

  return r;
}

int read(int fd, void *buf, int count)
{
  struct read_ipc tmp;
  struct message *fs_reply;
  virt_addr_t *channel_ptr;
  virt_addr_t channel;

  tmp.fd = fd;
  tmp.count = count > MAX_BUFFER ? MAX_BUFFER : count;

  msg_send(FS, READ_IPC, &tmp, sizeof(struct read_ipc));
  msg_receive(FS);

  fs_reply = &cpuinfo->msg_input[FS];

  channel_ptr = (virt_addr_t*)fs_reply->data;
  channel = *channel_ptr;

  memcpy(buf, (void*)channel, count);

  return tmp.count;
}

int close(int fd)
{
  struct close_ipc tmp;
  int r;

  tmp.fd = fd;

  msg_send(FS, CLOSE_IPC, &tmp, sizeof(struct close_ipc));
  msg_receive(FS);

  memcpy(&r, &cpuinfo->msg_input[FS].data, sizeof(int));

  return r;
}

#include <fs.h>
#include <ipc.h>
#include <string.h>
#include <config.h>

#include <lib_mem.h>
#include <debug.h>

void open_std()
{
  open("stdin", O_RDONLY);
  open("stdout", O_RDWR);
}

int open(const char *pathname, int flags)
{
  struct open_ipc tmp;
  struct message *fs_reply;
  struct open_reply *reply_data;
  int fd;
  size_t len;

  len = strlen(pathname) > MAX_PATHNAME ? MAX_PATHNAME : strlen(pathname);

  strncpy(tmp.pathname, pathname, len);
  tmp.pathname[len] = '\0';
  tmp.flags = flags;

  msg_send(FS, OPEN_IPC, &tmp, sizeof(struct open_ipc));
  msg_receive(FS);

  fs_reply = &cpuinfo->msg_input[FS];

  reply_data = (struct open_reply *)fs_reply->data;

  fd = reply_data->fd;

  if (fd < 0)
    return -1;

  return fd;
}

int read(int fd, void *buf, int count)
{
  struct read_ipc tmp;
  struct message *fs_reply;
  struct read_reply *reply_data;

  tmp.fd = fd;
  tmp.buf = (char*)virt2phys(cpuinfo, (virt_addr_t)buf);
  tmp.count = count;

  msg_send(FS, READ_IPC, &tmp, sizeof(struct read_ipc));
  msg_receive(FS);

  fs_reply = &cpuinfo->msg_input[FS];

  reply_data = (struct read_reply *)fs_reply->data;

  if (reply_data->type == TYPE_CHAR) {
    memcpy(buf, (void*)reply_data->channel, reply_data->count);
  }

  return reply_data->count;
}

int write(int fd, void *buf, int count)
{
  struct write_ipc tmp;
  struct message *fs_reply;
  struct write_reply *reply_data;
  struct file_descriptor *fds = cpuinfo->vm_info.vm_fds;

  tmp.fd = fd;
  tmp.buf = (void *)fds[fd].offset;
  tmp.count = count;

  memcpy((void *)fds[fd].offset, buf, count);

  msg_send(FS, WRITE_IPC, &tmp, sizeof(struct write_ipc));
  msg_receive(FS);

  fs_reply = &cpuinfo->msg_input[FS];

  reply_data = (struct write_reply *)fs_reply->data;

  return reply_data->count;
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

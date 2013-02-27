#include <fs.h>
#include <ipc.h>
#include <string.h>
#include <config.h>
#include <panic.h>
#include <channel.h>

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

int read_char(int fd, void *buf, int count)
{
  struct channel *cnl = (struct channel *)cpuinfo->vm_info.vm_fds[fd].offset;
  struct read_char_rq req;

  if (count >= (_4KB_ - offsetof(struct channel, data) - sizeof(struct read_char_rq))) {
    panic ("Count too large!");
  }

  req.count = count;

  memcpy(cnl->data, &req, sizeof (struct read_char_rq));

  cnl_send(cnl);
  cnl_receive((virt_addr_t)cnl);

  if (cnl->result != -1) {
    memcpy(buf, cnl->data, cnl->result);
  }

  return cnl->result;
}

int read(int fd, void *buf, int count)
{
  if (cpuinfo->vm_info.vm_fds[fd].type == TYPE_CHAR) {
    return read_char(fd, buf, count);
  }

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

  return reply_data->count;
}

int write_char(int fd, void *buf, int count)
{
  struct channel *cnl = (struct channel *)cpuinfo->vm_info.vm_fds[fd].offset;
  struct read_char_rq req;

  if (count >= (_4KB_ - offsetof(struct channel, data) - sizeof(struct read_char_rq))) {
    panic ("Count too large!");
  }

  req.count = count;

  memcpy(cnl->data, &req, sizeof (struct read_char_rq));
  memcpy(cnl->data + sizeof (struct read_char_rq), buf, count);

  cnl_send(cnl);
  cnl_receive((virt_addr_t)cnl);

  return cnl->result;
}

int write(int fd, void *buf, int count)
{
  if (cpuinfo->vm_info.vm_fds[fd].type == 0) {
    panic ("Type is worng!");
  }

  if (cpuinfo->vm_info.vm_fds[fd].type == TYPE_CHAR) {
    return write_char(fd, buf, count);
  } else {
    panic ("Only CHAR devices are available for write");
  }

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

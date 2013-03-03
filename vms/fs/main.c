#include <cdef.h>
#include <types.h>
#include <const.h>
#include <asmfunc.h>
#include <cpu.h>
#include <ipc.h>
#include <fs.h>
#include <string.h>
#include <panic.h>
#include <config.h>
#include <misc.h>
#include <pm.h>
#include <debug.h>

char *filesystem;

struct cpu_info *
get_cpu_info (cpuid_t cpuid)
{
  if (cpuid > MAX_CPUS) {
    return NULL;
  }

  struct cpu_info *base = (struct cpu_info *)((phys_addr_t)cpuinfo - (cpuinfo->cpuid * _4KB_));

  return (struct cpu_info *)((phys_addr_t)base + cpuid * _4KB_);
}

struct file_descriptor *
get_fds (int from)
{
  if (from > MAX_CPUS)
    return NULL;

  struct cpu_info *cpuinfo = get_cpu_info(from);

  return cpuinfo->vm_info.vm_fds;
}

void
local_open_char(const char *pathname, int from, struct open_reply *openreply)
{
  int fd;
  uint32_t dst;
  struct channel_ipc msg;
  struct message *pm_reply;
  struct file_descriptor *fds;
  int i;

  if (from > MAX_CPUS || from < 0) {
    openreply->fd = -1;
    return;
  }

  fds = get_fds(from);
  if (fds == 0) {
    openreply->fd = -1;
    return;
  }

  if (strcmp (pathname, "stdin") == 0) {
    fd = 0;
    dst = KBD;
  } else if (strcmp (pathname, "stdout") == 0) {
    fd = 1;
    dst = CONSOLE;
  } else if (strcmp (pathname, "junkfd") == 0) {
    // Find the lowest available fd
    for (i = 2 ; i < MAX_FD ; i++)
      if (fds[i].offset == 0)
        break;
    fd = i;
    dst = JUNK;
  } else {
    fd = -1;  /* JUST to SHUT UP GCC */
    DEBUG ("FS: Unknown char file!", 0x4);
    halt ();
  }

  /* If the file is already open! */
  if (fds[fd].offset != 0) {
    openreply->fd = -1;
    return;
  }

  msg.end1 = from;
  msg.end2 = dst;

  msg_send (PM, CHANNEL_IPC, &msg, sizeof (struct channel_ipc));

  msg_receive (PM);

  pm_reply = &cpuinfo->msg_input[PM];

  memcpy (&fds[fd].offset, pm_reply->data, sizeof (phys_addr_t));
  fds[fd].type = TYPE_CHAR;
  fds[fd].length = 0;
  fds[fd].dst = dst;

  openreply->fd = fd;
  openreply->channel = (void *)fds[fd].offset;
}

void
local_open(const char *pathname, int flags, int from, struct open_reply *openreply)
{
  int i;
  int fd;
  struct header *tmp;
  int num_files;
  struct file_descriptor *fds;

  if (strcmp(pathname, "stdin") == 0 || strcmp(pathname, "stdout") == 0 || strcmp(pathname, "junkfd") == 0)
    return local_open_char(pathname, from, openreply);

  fds = get_fds(from);
  if (fds == 0) {
    openreply->fd = -1;
    return;
  }

  // Find the lowest available fd
  for (i = 2 ; i < MAX_FD ; i++)
    if (fds[i].offset == 0)
      break;

  if (i >= MAX_FD) {
    openreply->fd = -1;
    return;
  }

  fd = i;

  // Only O_RDONLY is supported
  if (flags != O_RDONLY) {
    openreply->fd = -1;
    return;
  }

  // Search if the file exists
  num_files = (int)*filesystem;
  for (i = 0 ; i < num_files ; i++) {
    tmp = (struct header *)(filesystem + sizeof(int) + (i * sizeof(struct header)));
    if (strcmp(tmp->name, pathname) == 0)
      break;
  }

  if (i >= num_files) {
    openreply->fd = -1;
    return;
  }

  // Copy the structure
  // NOTE: We are not copying name for performance reasons.
  fds[fd].type = tmp->type;
  fds[fd].length = tmp->length;
  fds[fd].dst = 0;
  fds[fd].offset = tmp->offset;

  openreply->fd = fd;
  openreply->channel = NULL;
}

static int
local_read_char (int fd, int count, int from)
{
  struct keyboard_read kbd_rd;
  void *channel_addr;
  int to;
  struct file_descriptor *fds;

  if (fd > MAX_FD) {
    DEBUG ("local_read_char: TOO big fd value %d!", 0x4, fd);
    halt ();
  }
  if (from > MAX_CPUS || from < 0) {
    DEBUG  ("local_read_char: \"from\" = %d not valid!", 0x4, from);
    halt ();
  }

  to = (fd == 0) ? KBD : CONSOLE;

  fds = get_fds(from);
  if (fds == 0) {
    return 0;
  }

  channel_addr = (void *)fds[fd].offset;

  if (channel_addr == 0) {
    DEBUG ("local_read_char: No channle found!", 0x4);
    halt ();
  }

  kbd_rd.from = from;
  kbd_rd.channel = channel_addr;
  kbd_rd.count = count;

  msg_reply (from, to, READ_IPC, &kbd_rd, sizeof (struct keyboard_read));

  return count;
}

static int
local_read_file (int fd, void *buf, int count, int from)
{
  struct file_descriptor *fds;

  fds = get_fds(from);
  if (fds == 0) {
    return 0;
  }

  if (count > fds[fd].length)
    count = fds[fd].length;

  memcpy(buf, filesystem + fds[fd].offset, count);

  return count;
}

static int
get_type(int fd, int from)
{
  if (fd > MAX_FD || fd < 0)
    return -1;

  struct file_descriptor *fds;

  fds = get_fds(from);
  if (fds == 0) {
    return -1;
  }

  if (fds[fd].type == 0)
    return -1;

  return fds[fd].type;
}

int
local_read(int fd, void *buf, int count, int from)
{
  struct file_descriptor *fds;
  int type;

  type = get_type(fd, from);
  if (type < 0)
    return -1;

  fds = get_fds(from);
  if (fds == 0) {
    return -1;
  }

  if (fds[fd].type == TYPE_CHAR) {
    return local_read_char (fd, count, from);
  } else if (fds[fd].type == TYPE_FILE) {
    return local_read_file (fd, buf, count, from);
  }

  DEBUG ("FS: local_read: File type (%d) is unknown!\n", 0x4, fds[fd].type);
  halt();

  return -1;
}

void
local_write(int fd, void *buf, int count, int from)
{
  struct console_write cwrite;
  struct write_reply writereply;

  if (get_type(fd, from) == TYPE_FILE || fd != 1) {
    writereply.from = from;
    writereply.count = -1;
    msg_reply (FS, from, WRITE_IPC, &writereply, sizeof(struct write_reply));
  }

  cwrite.from = from;
  cwrite.channel = buf;
  cwrite.count = count;

  msg_reply (from, CONSOLE, WRITE_IPC, &cwrite, sizeof(struct console_write));
}

int
local_close(int fd, int from)
{
  struct file_descriptor *fds;
  if (fd > MAX_FD)
    return -1;

  fds = get_fds(from);
  if (fds == 0) {
    return 0;
  }

  fds[fd].type = 0;
  fds[fd].length = 0;
  fds[fd].offset = 0;

  return 0;
}

phys_addr_t
local_load(char *path)
{
  int i;
  struct header *tmp;
  int num_files;

  // Search if the file exists
  num_files = (int)*filesystem;
  for (i = 0 ; i < num_files ; i++) {
    tmp = (struct header *)(filesystem + sizeof(int) + (i * sizeof(struct header)));
    if (strcmp(tmp->name, path) == 0)
      break;
  }
  if (i >= num_files)
    return 0;

  return (phys_addr_t)(filesystem + tmp->offset);
}

void
vm_main (void)
{
  filesystem = cpuinfo->vm_args;

#if 0
  int i;
  for (i = 0 ; i < cpuinfo->cpuid; i++) DEBUG ("\n", 0x7);
  DEBUG  ("FS: My info is in addr = %d\n", 0xD, cpuinfo->cpuid);
  halt ();
#endif

  while (1) {
    struct message *m = msg_check();
    struct open_ipc opentmp;
    struct open_reply openreply;
    struct read_ipc readtmp;
    struct read_reply readreply;
    struct read_reply *reply;
    struct close_ipc closetmp;
    struct write_ipc *writeipc;
    struct write_reply *writereply;
    int r;
    phys_addr_t f;

    switch(m->number) {
    case OPEN_IPC:
      if (is_driver(m->from)) {
        r = -1;
        msg_reply(FS, m->from, OPEN_IPC, &r, sizeof(int));
        break;
      }
      memcpy(&opentmp, m->data, sizeof(struct open_ipc));
      local_open(opentmp.pathname, opentmp.flags, m->from, &openreply);
      msg_reply(FS, m->from, OPEN_IPC, &openreply, sizeof(struct open_reply));
      break;
    case READ_IPC:
      memcpy(&readtmp, m->data, sizeof(struct read_ipc));
      r = local_read(readtmp.fd, readtmp.buf, readtmp.count, m->from);
      if (get_type(readtmp.fd, m->from) == TYPE_FILE) {
        readreply.type = TYPE_FILE;
        readreply.count = r;
	readreply.channel = NULL;
        msg_reply(FS, m->from, READ_IPC, &readreply, sizeof(struct read_reply));
      }
      break;
    case CLOSE_IPC:
      memcpy(&closetmp, m->data, sizeof(struct close_ipc));
      r = local_close(closetmp.fd, m->from);
      msg_reply(FS, m->from, CLOSE_IPC, &r, sizeof(int));
      break;
    case LOAD_IPC:
      if (m->from != PM) {
        f = 0;
        msg_reply(FS, m->from, LOAD_IPC, &f, sizeof(phys_addr_t));
        break;
      }
      f = local_load(m->data);
      msg_reply(FS, m->from, LOAD_IPC, &f, sizeof(phys_addr_t));
      break;
    case WRITE_IPC:
      writeipc = (struct write_ipc*)m->data;
      local_write(writeipc->fd, writeipc->buf, writeipc->count, m->from);
      break;
    case READ_ACK:
      reply = (struct read_reply *)m->data;
      msg_reply(FS, reply->from, READ_IPC, reply, sizeof(struct read_reply));
      msg_reply(FS, KBD, READ_IPC, reply, sizeof(struct read_reply));
      break;
    case WRITE_ACK:
      writereply = (struct write_reply *)m->data;
      msg_reply(FS, writereply->from, WRITE_IPC, writereply, sizeof(struct write_reply));
      msg_reply(FS, CONSOLE, WRITE_IPC, writereply, sizeof(struct write_reply));
      break;
    default:
      DEBUG ("FS: Warning, unknown request %d\n", 0xD, m->number);
    }
  }

  while (1) {__asm__ __volatile__ ("cli;hlt;\n\t");}
}

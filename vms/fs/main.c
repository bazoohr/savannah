#include <cdef.h>
#include <types.h>
#include <printk.h>
#include <console.h>
#include <const.h>
#include <asmfunc.h>
#include <cpu.h>
#include <ipc.h>
#include <fs.h>
#include <string.h>
#include <panic.h>
#include <config.h>
#include <con.h>
#include <misc.h>

struct header {
	char name[32];        /* File name */
  uint32_t type;    /* Type of the file (normal file, char, block ...) */
	uint32_t length;  /* File length */
	uint64_t offset;  /* Offset where the file is located starting from
			       * the beginning of the file */
};

char *filesystem;

#define MAX_FD (64)

struct header fds[MAX_CPUS][MAX_FD];

int
local_open_char(const char *pathname, int from)
{
  return -1;
}

int
local_open(const char *pathname, int flags, int from)
{
  int i;
  int fd;
  struct header *tmp;
  int num_files;

  if (strcmp(pathname, "stdin") == 0 || strcmp(pathname, "stdout") == 0)
    return local_open_char(pathname, from);

  // Find the lowest available fd
  for (i = 2 ; i < MAX_FD ; i++)
    if (fds[from][i].offset == 0)
      break;

  if (i >= MAX_FD)
    return -1;

  fd = i;

  // Only O_RDONLY is supported
  if (flags != O_RDONLY)
    return -1;

  // Search if the file exists
  num_files = (int)*filesystem;
  for (i = 0 ; i < num_files ; i++) {
    tmp = (struct header *)(filesystem + sizeof(int) + (i * sizeof(struct header)));
    if (strcmp(tmp->name, pathname) == 0)
      break;
  }
  if (i >= num_files)
    return -1;

  // Copy the structure
  // NOTE: We are not copying name for performance reasons.
  fds[from][fd].type = tmp->type;
  fds[from][fd].length = tmp->length;
  fds[from][fd].offset = tmp->offset;

  return fd;
}

int
local_read(int fd, void *buf, int count, int from)
{
  struct header tmp = fds[from][fd];

  if (count > tmp.length)
    count = tmp.length;
  if (count > MAX_BUFFER)
    count = MAX_BUFFER;

  memcpy(buf, filesystem + tmp.offset, count);

  return count;
}

int
local_close(int fd, int from)
{
  if (fd > MAX_FD)
    return -1;

  fds[from][fd].type = 0;
  fds[from][fd].length = 0;
  fds[from][fd].offset = 0;

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
  int i;
  con_init ();

  filesystem = cpuinfo->vm_args;

  for (i = 0 ; i < cpuinfo->cpuid; i++) printk("\n");

  cprintk ("FS: My info is in addr = %d\n", 0xD, cpuinfo->cpuid);

  while (1) {
    struct message *m = msg_check();
    struct open_ipc opentmp;
    struct read_ipc readtmp;
    struct close_ipc closetmp;
    int r;
    phys_addr_t f;

    switch(m->number) {
    case OPEN_IPC:
      if (is_driver(m->from)) {
        r = -1;
        msg_reply(m->from, OPEN_IPC, &r, sizeof(int));
        break;
      }
      memcpy(&opentmp, m->data, sizeof(struct open_ipc));
      r = local_open(opentmp.pathname, opentmp.flags, m->from);
      msg_reply(m->from, OPEN_IPC, &r, sizeof(int));
      break;
    case READ_IPC:
      memcpy(&readtmp, m->data, sizeof(struct read_ipc));
      r = local_read(readtmp.fd, &readtmp.buf, readtmp.count, m->from);
      readtmp.count = r;
      msg_reply(m->from, READ_IPC, &readtmp, sizeof(struct read_ipc));
      break;
    case CLOSE_IPC:
      memcpy(&closetmp, m->data, sizeof(struct close_ipc));
      r = local_close(closetmp.fd, m->from);
      msg_reply(m->from, CLOSE_IPC, &r, sizeof(int));
      break;
    case LOAD_IPC:
      if (m->from != PM) {
        f = 0;
        msg_reply(m->from, LOAD_IPC, &f, sizeof(phys_addr_t));
        break;
      }
      f = local_load(m->data);
      msg_reply(m->from, LOAD_IPC, &f, sizeof(phys_addr_t));
      break;
    case PUTC_IPC:
      if (m->from != CONSOLE) {
        msg_reply(CONSOLE, PUTC_IPC, m->data, sizeof(struct putc_ipc));
      } else {
        msg_reply(m->from, PUTC_IPC, m->data, sizeof(int));
      }
      break;
    default:
      cprintk("FS: Warning, unknown request %d\n", 0xD, m->number);
    }
  }

  while (1) {__asm__ __volatile__ ("cli;hlt;\n\t");}
}

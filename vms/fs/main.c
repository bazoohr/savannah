#include <cdef.h>
#include <dev/pic.h>
#include <types.h>
#include <printk.h>
#include <console.h>
#include <const.h>
#include <interrupt.h>
#include <asmfunc.h>
#include <memory.h>
#include <dev/keyboard.h>
#include <mp.h>
#include <dev/ioapic.h>
#include <dev/lapic.h>
#include <cpu.h>
#include <ipc.h>
#include <fs.h>
#include <string.h>
#include <panic.h>

struct header {
  char name[32];
  unsigned int length;
  unsigned int offset;
};

char filesystem[] = { 0x02, 0x00, 0x00, 0x00, 0x31, 0x2e, 0x74, 0x78, 0x74, 0x00, 0x04, 0x08,
                      0xf4, 0xdf, 0x75, 0xb7, 0xf6, 0x5e, 0x60, 0xb7, 0xe8, 0x9a, 0x04, 0x08,
                      0x63, 0x88, 0x04, 0x08, 0x40, 0x88, 0x04, 0x08, 0x40, 0x84, 0x04, 0x08,
                      0x04, 0x00, 0x00, 0x00, 0x54, 0x00, 0x00, 0x00, 0x32, 0x2e, 0x74, 0x78,
                      0x74, 0x00, 0x04, 0x08, 0x8e, 0xff, 0x77, 0x01, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x99, 0x5e, 0xb7, 0xd0, 0x73, 0x78, 0xb7, 0x14, 0xe3, 0xaf, 0xbf,
                      0xb3, 0xff, 0x60, 0xb7, 0x04, 0x00, 0x00, 0x00, 0x58, 0x00, 0x00, 0x00,
                      0x66, 0x6f, 0x6f, 0x0a, 0x62, 0x61, 0x72, 0x0a  };

#define MAX_FD (64)

struct header fds[MAX_FD];

int
local_open(const char *pathname, int flags)
{
  int i;
  int fd;
  struct header *tmp;
  int num_files;

  // Find the lowest available fd
  for (i = 0 ; i < MAX_FD ; i++)
    if (fds[i].length == 0)
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
  strcpy(fds[fd].name, tmp->name);
  fds[fd].length = tmp->length;
  fds[fd].offset = tmp->offset;

  return fd;
}

int
local_read(int fd, void *buf, int count)
{
  struct header tmp = fds[fd];

  if (count > tmp.length)
    count = tmp.length;
  if (count > MAX_BUFFER)
    count = MAX_BUFFER;

  memcpy(buf, filesystem + tmp.offset, count);

  return count;
}

int
local_close(int fd)
{
  if (fd > MAX_FD)
    return -1;

  strcpy(fds[fd].name, "");
  fds[fd].length = 0;
  fds[fd].offset = 0;

  return 0;
}

void
vm_main (void)
{
  int i;
  con_init ();

  for (i = 0 ; i < cpuinfo->cpuid; i++) printk("\n");

  cprintk ("FS: My info is in addr = %d\n", 0xD, cpuinfo->cpuid);

  while (1) {
    struct message *m = msg_check();
    struct open_ipc opentmp;
    struct read_ipc readtmp;
    struct close_ipc closetmp;
    int r;

    switch(m->number) {
    case OPEN_IPC:
      memcpy(&opentmp, &m->data, sizeof(struct open_ipc));
      r = local_open(opentmp.pathname, opentmp.flags);
      msg_reply(m->from, OPEN_IPC, &r, sizeof(int));
      break;
    case READ_IPC:
      memcpy(&readtmp, &m->data, sizeof(struct read_ipc));
      r = local_read(readtmp.fd, &readtmp.buf, readtmp.count);
      readtmp.count = r;
      msg_reply(m->from, READ_IPC, &readtmp, sizeof(struct read_ipc));
      break;
    case CLOSE_IPC:
      memcpy(&closetmp, &m->data, sizeof(struct close_ipc));
      r = local_close(closetmp.fd);
      msg_reply(m->from, CLOSE_IPC, &r, sizeof(int));
      break;
    default:
      cprintk("FS: Warning, unknown request %d\n", 0xD, m->number);
    }
  }

  while (1) {__asm__ __volatile__ ("cli;hlt;\n\t");}
}

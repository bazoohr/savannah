#include <printf.h>
#include <string.h>
#include <cpuinfo.h>
#include <fs.h>
#include <channel.h>
#include <panic.h>

#define NUM 5
#define COUNT 4

void aread(int fd, int count)
{
  struct channel *cnl = (struct channel *)cpuinfo->vm_info.vm_fds[fd].offset;
  struct read_char_rq req;

  if (count >= (_4KB_ - offsetof(struct channel, data) - sizeof(struct read_char_rq))) {
    panic ("Count too large!");
  }

  req.count = count;

  memcpy(cnl->data, &req, sizeof (struct read_char_rq));

  cnl_send(cnl);
}

int aread_receive(int fd, void *buf)
{
  struct channel *cnl = (struct channel *)cpuinfo->vm_info.vm_fds[fd].offset;

  cnl_receive((virt_addr_t)cnl);

  if (cnl->result != -1) {
    memcpy(buf, cnl->data + sizeof(struct read_char_rq), cnl->result);
  }

  return cnl->result;
}

int main(int argc, char **argv)
{
  int fds[NUM];
  char buf[NUM][10];
  int i, n;

  for (i = 0 ; i < NUM ; i++) {
    fds[i] = open("junkfd", O_RDONLY);
    if (fds[i] == -1) {
      printf("Open %d failed\n", i);
    } else {
      printf("fds[%d] = %d\n", i, fds[i]);
    }
  }

  for (i = 0 ; i < NUM ; i++) {
    if (fds[i] != -1) {
      aread(fds[i], COUNT);
    }
  }

  for (i = 0 ; i < NUM ; i++) {
    if (fds[i] != -1) {
      n = aread_receive(fds[i], buf[i]);
      buf[i][n] = '\0';
      printf("%d : %s\n", i, buf[i]);
    }
  }

  return 0;
}

#include <printf.h>
#include <fs.h>
#include <string.h>
#include <misc.h>

int main(int argc, char **argv)
{
  char content[10];
  int fd, fd2;

  memset(content, 0, 10);

  fd = open("2.txt", O_RDONLY);
  if (fd == -1)
    printf ("Wrong fd\n");

  read(fd, content, 10);

  printf ("Content 1: %s\n", content);

  //close(fd);

  fd2 = open("1.txt", O_RDONLY);
  if (fd2 == -1)
    printf ("Wrong fd\n");

  read(fd2, &content, 10);

  printf ("Content 2: %s\n", content);

  printf ("Fd1 = %d\n", fd);
  printf ("Fd2 = %d\n", fd2);

  return 0;
}

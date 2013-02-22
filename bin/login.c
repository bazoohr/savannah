#include <fs.h>
#include <pm.h>
#include <printf.h>

#define MAX_LEN 16

int main(int argc, char **argv)
{
  int i, ch, pid, r;
  char cmd[MAX_LEN];

  open_std();

  printf("$ ");

  i = 0;
  while (ch != '\n' && i < MAX_LEN) {
    read (0, &ch, 1);
    cmd[i] = ch;
    i++;
  }
  cmd[i] = '\0';

  printf("Command: %s\n", cmd);

  pid = fork();
  if (pid < 0) {
    printf("Fork error!\n");
  } else if (pid == 0) { /* Child */
    exec("test", NULL);
    printf("File not found\n");
    exit(-1);
  } else { /* Parent */
    waitpid (pid, &r, 0);
    printf("Parent\n");
  }

  return -1;
}


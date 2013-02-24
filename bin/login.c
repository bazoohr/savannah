#include <fs.h>
#include <pm.h>
#include <printf.h>
#include <string.h>

#define MAX_LEN 16

int main(int argc, char **argv)
{
  int i, ch, pid, r;
  char cmd[MAX_LEN];

  open_std();

  while (1) {
    printf("$ ");

    i = 0;
    ch = 0;
    while (ch != '\n' && i < MAX_LEN) {
      read (0, &ch, 1);
      printf("%c", ch);
      cmd[i] = ch;
      i++;
    }
    cmd[i-1] = '\0';

    if (strcmp(cmd, "exit") == 0) {
      break;
    }

    pid = fork();
    if (pid < 0) {
      printf("Fork error!\n");
    } else if (pid == 0) { /* Child */
      exec(cmd, NULL);
      printf("File not found\n");
      exit(-1);
    } else { /* Parent */
      waitpid (pid, &r, 0);
      printf("Return value = %d\n", r);
    }
  }

  return -1;
}


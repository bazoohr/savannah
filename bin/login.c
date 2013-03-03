#include <fs.h>
#include <pm.h>
#include <printf.h>
#include <string.h>
/* =================================== */
#define MAX_LEN 16
/* =================================== */
static inline void
readline (char *buf, size_t size)
{
  int i = 0;
  char ch = 0;

  while (ch != '\n' && i < size) {
    read (0, &ch, 1);
    printf ("%c", ch);
    buf[i] = ch;
    i++;
  }
  buf[i-1] = '\0';
}
/* =================================== */
static inline void
welcome (void)
{
  printf ("\n");
  printf ("* ============================================ *\n");
  printf ("* Welcome to VUOS                              *\n");
  printf ("* ============================================ *\n");
}
/* =================================== */
static inline void
login (void)
{
  char user[16];

  while (1) {
    printf ("username: ");
    readline (user, 16);
    if (strcmp ("root", user) == 0) {
      break;
    }
    printf ("Unknow user name!\n");
  }
}
/* =================================== */
int main(int argc, char **argv)
{
  int pid, r;
  char cmd[MAX_LEN];

  open_std ();

  welcome ();
  login ();

  while (1) {
    printf("~:# ");

    readline (cmd, MAX_LEN);
    if (strlen (cmd) == 0)
      continue;

    if (strcmp(cmd, "exit") == 0) {
      break;
    }

    pid = fork();
    if (pid < 0) {
      printf("Fork error!\n");
    } else if (pid == 0) { /* Child */
      exec(cmd, NULL);
      printf ("Command not found\n");
      exit (-1);
    } else { /* Parent */
      waitpid (pid, &r, 0);
      printf("returned %d\n", r);
    }
  }

  return -1;
}


#include <fs.h>
#include <pm.h>
#include <printf.h>
#include <string.h>
#include <asmfunc.h>
#include <debug.h>
#include <ipc.h>
/* =================================== */
#define MAX_LEN 16
/* =================================== */
#if 0
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
#endif
/* =================================== */
struct fake {
  char none[MSG_DATA_SIZE];
} __packed;
struct fake tmp;
int main(int argc, char **argv)
{
#if 0
  int pid, r;
  char cmd[MAX_LEN];


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
#endif
  int a, b;
  uint64_t sum = 0;
  int i;
  int pid;
  unsigned int aux = cpuinfo->cpuid;

  DEBUG ("\n\n\n", 0x7);

#ifdef MAX
#undef MAX
#endif
#define MAX 10
  sum = 0;
  for (i = 0; i < MAX; i++) {
    b = rdtscp (&aux);
    msg_send(PM, 999, &tmp, sizeof(struct fake));
    msg_receive(PM);
    a = rdtscp (&aux);
    sum += (a - b);
  }
  DEBUG ("ferq = %x empty took %d in average!\n", 0xE, cpuinfo->cpu_freq, sum / MAX);
#ifdef MAX
#undef MAX
#endif
#define MAX 10
  sum = 0;
  for (i = 0; i < MAX; i++) {
    b = rdtscp (&aux);
    pid = fork();
    if (pid < 0) {
      if (i == 0) {
        DEBUG ("First fork", 0x4);
      }
      DEBUG ("Fork error!\n", 0x4);
    } else if (pid == 0) { /* Child */
      exit (-1);
    } else { /* Parent */
      int r;
      a = rdtscp (&aux);
      sum += (a - b);
      waitpid (pid, &r, 0);
      //DEBUG ("returned %d\n", 0xA, r);
    }
  }

  DEBUG ("ferq = %x fork took %d in average!\n", 0xE, cpuinfo->cpu_freq, sum / MAX);
#ifdef MAX
#undef MAX
#endif
#define MAX 10
  sum = 0;
  for (i = 0; i < MAX; i++) {
    b = rdtscp (&aux);
    empty();
    a = rdtscp (&aux);
    sum += (a - b);
  }
  DEBUG ("ferq = %x empty took %d in average!\n", 0xE, cpuinfo->cpu_freq, sum / MAX);
  halt ();

  return -1;
}


#include <cdef.h>
#include <thread.h>
#include <proc.h>
#include <io.h>
#include <asmlib.h>
#include <malloc.h>
#include <string.h>
/* ===================================== */
void * __unused
func (void *args)
{
  int color = *(int*)args;
  cprintf ("Thread: Press a key:", color);
  int c = getch ();
  cprintf ("Thread: %c\n", color, c);
  return NULL;
}
/* ===================================== */
int main (void)
{
  int i = 0;

  while (1) {
    pid_t pid;
    tid_t tid;

    pid = fork ();
    if (pid) {
      int color = 0x6;
      tid = thread_create (&func, &color);
      if (tid < 0) {
        cprintf ("failed to create thread!\n", 0x4);
        for (;;);
      }
      cprintf ("Parent:Press Something", 0xA);
      thread_join (tid);
      int status;
      waitpid (-1, &status);
      cprintf ("child exited with statys %d\n", 0xE, status);
      int c = getch ();
      cprintf ("Parent: %c\n", 0xA, c);
      i++;
    } else {
      char *a;

      a = (char *)malloc (0x1000);
      if (!a) {
        cprintf ("failed to allocate memory!\n", 0x4);
        for (;;);
      }

      memset (a, 0xA, 0x1000);
      cprintf ("Child: Press a Key", 0xC);
      int c = getch ();
      cprintf ("Child: %c\n", 0xC, c);
      free (a);
      return i;
    }
  }
}

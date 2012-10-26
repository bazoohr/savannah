#include <io.h>
#include <asmlib.h>

#define DEV_KBD 0

int
getch (void)
{
  int ch;
  int nbytes;

  nbytes =  read (DEV_KBD, (char*)&ch, sizeof (int));

  return nbytes > 0 ? ch : -1;
}

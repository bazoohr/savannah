#include <stdlib.h>
int
atoi (const char *s)
{
  int i = 0;

  while (isdigit (*s)) {
    i = i * 10 + *s++ - '0';
  }
  return i;
}


#include <types.h>

void bcopy (const void *src, void *dst, size_t count)
{
  register int i;

  for (i = 0; i < count; i++) {
    ((uint8_t*)dst)[i] = ((uint8_t*)src)[i];
  }
}

void *memset (void *ptr, int c, size_t n)
{
  register int i;

  for (i = 0; i < n; i++) {
    ((char*)ptr)[i] = (char)c;
  }

  return ptr;
}


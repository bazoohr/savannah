#include <types.h>

void *
memcpy (void *dst, const void *src, size_t count)
{
  char *tmp = dst;
  const char *s = src;

  while (count--) {
    *tmp++ = *s++;
  }

  return dst;
}

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


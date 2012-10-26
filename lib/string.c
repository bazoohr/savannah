#include <string.h>

void
bcopy (register const void *src, void *dst, size_t count)
{
  register int i;

  for (i = 0; i < count; i++) {
    ((uint8_t*)dst)[i] = ((uint8_t*)src)[i];
  }
}

void *
memcpy (register void *dest, register const void *src, size_t size)
{
	register int i = 0;

	for (i = 0; i < size; i++)
		((uint8_t*)dest)[i] = ((uint8_t*)src)[i];

	return dest;
}

void *
memset (register void *ptr, int c, size_t n)
{
  register int i;

  for (i = 0; i < n; i++) {
    ((uint8_t*)ptr)[i] = (uint8_t)c;
  }

  return ptr;
}

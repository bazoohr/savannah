#include <string.h>
#include <asmfunc.h>

void * 
memcpy (void *dst, void *src, size_t count)
{
  return fast_memcpy (dst, src, count);
}

void *
memset (void *ptr, int c, size_t count)
{
  return fast_memset (ptr, c, count);
}
/*
 * This code is shamelessly stolen from Linux
 */
int
memcmp(const void *cs, const void *ct, size_t count)
{
	const unsigned char *su1, *su2;
	int res = 0;

	for (su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--)
		if ((res = *su1 - *su2) != 0)
			break;
	return res;
}


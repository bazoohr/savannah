#include <string.h>

void * 
memcpy (void *dst, const void *src, size_t count)
{
	char *tmp = dst;
	const char *s = src;

	while (count--)
		*tmp++ = *s++;

	return dst;
}

void *
memset (void *ptr, int c, size_t count)
{
	char *xs = ptr;

	while (count--)
		*xs++ = c;

	return ptr;
}

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


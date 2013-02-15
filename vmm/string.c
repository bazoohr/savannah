#include <string.h>

void *
memcpy (void *dest,
        const void *src,
        asize_t count)
{
	char *tmp __aligned (0x10)= dest;
	const char *s __aligned (0x10) = src;

	while (count--)
		*tmp++ = *s++;
	return dest;
}

void *
memset (void *s,
        int c,
        asize_t count)
{
	char *xs __aligned (0x10)= s;

	while (count--)
		*xs++ = c;
	return s;
}

char *
strcpy (char *dest,
        const char *src)
{
	char *tmp __aligned (0x10)= dest;

	while ((*dest++ = *src++) != '\0')
		/* nothing */;
	return tmp;
}

char *
strncpy (char *dest,
         const char *src,
         asize_t count)
{
	char *tmp __aligned (0x10)= dest;

	while (count) {
		if ((*tmp = *src) != 0)
			src++;
		tmp++;
		count--;
	}
	return dest;
}

int strlen (const char *s)
{
	const char *sc __aligned (0x10);

	for (sc = s; *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}

int strcmp (const char *cs,
           const char *ct)
{
	auint8_t c1, c2;

	while (1) {
		c1 = *cs++;
		c2 = *ct++;
		if (c1 != c2)
			return c1 < c2 ? -1 : 1;
		if (!c1)
			break;
	}
	return 0;
}


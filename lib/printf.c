#include <stdarg.h>
#include <printf.h>
#include <string.h>
#include <fs.h>

void
putc (int ch)
{
	write(1, &ch, 1);
}

void
puts (char *str)
{
	write(1, str, strlen(str));
}

static void
put_hex (uint64_t num)
{
	aint i;
	char digits[] __aligned (0x10) = "0123456789ABCDEF";
	char buffer[16] __aligned (0x10);

	memset (buffer, 0, sizeof buffer);

	for (i = sizeof (uint64_t) * 2 - 1; i >= 0; i--) {
		buffer[i] = digits[num >> (i << 2) & 0xF];
	}

	i = 15;
	while (buffer[i] == '0') i--;
	if (i == -1) {
		putc ('0');
	}
	while (i >= 0) {
		putc (buffer[i--]);
	}
}

static void
sn_put_hex (char *buf, uint64_t num, int *idx, const size_t max)
{
	aint i;
	char digits[] __aligned (0x10) = "0123456789ABCDEF";
	char buffer[16] __aligned (0x10);

	memset (buffer, 0, sizeof buffer);

	for (i = sizeof (uint64_t) * 2 - 1; i >= 0; i--) {
		buffer[i] = digits[num >> (i << 2) & 0xF];
	}

	i = 15;
	while (buffer[i] == '0') i--;
	if (*idx < max && i == -1) {
    buf[*idx] = '0';
    (*idx)++;
	}
	while (*idx < max && i >= 0) {
		buf[*idx] = buffer[i--];
    (*idx)++;
	}
}
static void
put_decimal (int num)
{
	char buffer[20] __aligned (0x10);
	bool negative __aligned (0x10) = false;
	aint pos = 0;
	aint i;

	if (num < 0) {
		num = -num;
		negative = true;
	}

	while (num / 10 > 0) {
		buffer[pos++] = num - num / 10 * 10;
		num /= 10;
	}

	buffer[pos] = num;

	if (negative == true) putc ('-');

	for (i = pos; i >= 0; i--) {
		putc (buffer[i] + '0');
	}
}

static void
sn_put_decimal (char *buf, int num, int *idx, const size_t max)
{
  char buffer[20] __aligned (0x10);
  bool negative __aligned (0x10) = false;
  aint pos = 0;
  aint i;

  if (num < 0) {
    num = -num;
    negative = true;
  }

  while (num / 10 > 0) {
    buffer[pos++] = num - num / 10 * 10;
    num /= 10;
  }

  buffer[pos] = num;

  if (*idx < max && negative == true) {
    buf[*idx] = '-';
    (*idx)++;
  }

  for (i = pos; *idx < max && i >= 0; i--) {
    buf[*idx] = buffer[i] + '0';
    (*idx)++;
  }
}

void snprintf (char *buf, const char *fmt, const size_t n, ...)
{
	va_list p __aligned (0x10);
	char ch __aligned (0x10);
  int i;

  if (n == 0)
    return;

  if (n == 1) {
    buf[0] = '\0';
    return;
  }

	va_start (p, n);

  i = 0;
  while ((ch = *fmt++) != '\0' && i < n - 1){
    if (ch != '%'){
      buf[i++] = ch;
      continue;
    }

    switch (*fmt++) {
      case 'c':
        buf[i] = (char)(va_arg (p, int));
        break;
      case 'x':
        sn_put_hex (buf, va_arg (p, long), &i, n - 1);
        break;
      case 'd':
        sn_put_decimal (buf, va_arg (p, int), &i, n - 1);
        break;
      case 's': {
                  char *tmp = (char*)va_arg (p, char*);
                  while (i < n - 1 && (buf[i++] = *tmp++));
                  break;
                }
      default:
                break;
    }
  }

  buf[i] = '\0';

	va_end (p);
}

void printf (const char* fmt, ...)
{
	va_list p __aligned (0x10);
	char ch __aligned (0x10);

	va_start (p, fmt);

	while ((ch = *fmt++) != '\0') {
		if (ch != '%'){
			putc (ch);
			continue;
		}

		switch (*fmt++){
			case 'c':
				putc (va_arg (p, int));
				break;
			case 'x':
				put_hex (va_arg (p, long));
				break;
			case 'd':
				put_decimal (va_arg (p, int));
				break;
			case 's':
				puts (va_arg (p, char*));
				break;
			default:
				break;
		}
	}

	va_end (p);
}

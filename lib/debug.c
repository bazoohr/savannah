#ifdef __DEBUG__
/* ======================================= */
#include <stdarg.h>
#include <debug_console.h>
#include <string.h>
/* ======================================= */
static void put_hex (uint64_t num, int color)
{
	aint i;
	char digits[] __aligned (0x10)= "0123456789ABCDEF";
  char buffer[16] __aligned (0x10);

  memset (buffer, 0, sizeof buffer);

	for (i = sizeof (uint64_t) * 2 - 1; i >= 0; i--) {
		buffer[i] = digits[num >> (i << 2) & 0xF];
  }

  i = 15;
  while (buffer[i] == '0') i--;
  if (i == -1) {
    debug_con_putc ('0', color);
  }
  while (i >= 0) {
    debug_con_putc (buffer[i--], color);
  }
}

static void put_decimal (int num, int color)
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

 if (negative == true) debug_con_putc ('-', color);

 for (i = pos; i >= 0; i--) {
   debug_con_putc (buffer[i] + '0', color);
 }
}

void
print_debug_info (const char* fmt, int color, ...)
{
 	va_list p __aligned (0x10);
	char ch __aligned (0x10);

	va_start (p, color);

	while ((ch = *fmt++) != '\0'){

		if (ch != '%'){
			debug_con_putc (ch, color);
			continue;
		}

		switch (*fmt++){
			case 'c':
				debug_con_putc (va_arg (p, int), color);
				break;
			case 'x':
				put_hex (va_arg (p, long), color);
				break;
      case 'd':
        put_decimal (va_arg (p, int), color);
        break;
			case 's':
				debug_con_puts (va_arg (p, char*), color);
				break;
			default:
        break;
		}
	}

	va_end (p);
}

void
print_debugf_info (const char* fmt, ...)
{
 	va_list p __aligned (0x10);
	char ch __aligned (0x10);
  int color = 0xF;

	va_start (p, fmt);

	while ((ch = *fmt++) != '\0'){

		if (ch != '%'){
			debug_con_putc (ch, color);
			continue;
		}

		switch (*fmt++){
			case 'c':
				debug_con_putc (va_arg (p, int), color);
				break;
			case 'x':
				put_hex (va_arg (p, long), color);
				break;
      case 'd':
        put_decimal (va_arg (p, int), color);
        break;
			case 's':
				debug_con_puts (va_arg (p, char*), color);
				break;
			default:
        break;
		}
	}

	va_end (p);
}
#endif

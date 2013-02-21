#include <stdarg.h>
#include <console.h>
#include <string.h>
/* ============================================== */
static void
put_hex (uint64_t num, int color)
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
    con_putc ('0', color);
  }
  while (i >= 0) {
    con_putc (buffer[i--], color);
  }
}
/* ============================================== */
static void
put_decimal (int num, int color)
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

 if (negative == true) con_putc ('-', color);

 for (i = pos; i >= 0; i--) {
   con_putc (buffer[i] + '0', color);
 }
}
/* ============================================== */
static void
vcprintf (const char* fmt, int color, va_list ap)
{
	char ch __aligned (0x10);

	while ((ch = *fmt++) != '\0'){

		if (ch != '%'){
			con_putc (ch, color);
			continue;
		}

		switch (*fmt++){
			case 'c':
				con_putc (va_arg (ap, int), color);
				break;
			case 'x':
				put_hex (va_arg (ap, long), color);
				break;
      case 'd':
        put_decimal (va_arg (ap, int), color);
        break;
			case 's':
				con_puts (va_arg (ap, char*), color);
				break;
			default:
        break;
		}
	}
}
/* ============================================== */
void
panic (const char *fmt, ...)
{
  va_list ap;

	va_start (ap, fmt);

  vcprintf (fmt, 0x4, ap);

	va_end (ap);

  __asm__ __volatile__ ("cli;hlt\n");
}

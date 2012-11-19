#include <stdarg.h>
#include <printk.h>
#include <string.h>

static void put_hex (uint64_t num, int color)
{
	int i;
	char digits[] = "0123456789ABCDEF";
  char buffer[16];

  memset (buffer, 0, sizeof buffer);

	for (i = sizeof (uint64_t) * 2 - 1; i >= 0; i--) {
		buffer[i] = digits[num >> (i << 2) & 0xF];
  }
  
  i = 15;
  while (buffer[i] == '0') i--;
  if (i == -1) {
    kputc ('0', color);
  }
  while (i >= 0) {
    kputc (buffer[i--], color);
  }
}

static void put_decimal (int num, int color)
{
  char buffer[20];
  bool negative = false;
  int pos = 0;
  int i;

  if (num < 0) {
    num = -num;
    negative = true;
  }

 while (num / 10 > 0) {
   buffer[pos++] = num - num / 10 * 10;
   num /= 10;
 }

 buffer[pos] = num;

 if (negative == true) kputc ('-', color);

 for (i = pos; i >= 0; i--) {
   kputc (buffer[i] + '0', color);
 }
}
void printk (const char* fmt, ...)
{
	va_list p;
	char ch;

	va_start (p, fmt);

	while ((ch = *fmt++) != '\0'){

		if (ch != '%'){
			kputc (ch, TEXT_COLOR_WHITE);
			continue;
		}

		switch (*fmt++){
			case 'c':
				kputc (va_arg (p, int), TEXT_COLOR_WHITE);
				break; 
			case 'x':
				put_hex (va_arg (p, long), TEXT_COLOR_WHITE);
				break;
      case 'd':
        put_decimal (va_arg (p, int), TEXT_COLOR_WHITE);
        break;
			case 's':
				kputs (va_arg (p, char*), TEXT_COLOR_WHITE);
				break;
			default:
        break;
		}
	}

	va_end (p);
}

void cprintk (const char* fmt, int color, ...)
{
 	va_list p;
	char ch;

	va_start (p, color);

	while ((ch = *fmt++) != '\0'){

		if (ch != '%'){
			kputc (ch, color);
			continue;
		}

		switch (*fmt++){
			case 'c':
				kputc (va_arg (p, int), color);
				break; 
			case 'x':
				put_hex (va_arg (p, long), color);
				break;
      case 'd':
        put_decimal (va_arg (p, int), color);
        break;
			case 's':
				kputs (va_arg (p, char*), color);
				break;
			default:
        break;
		}
	}

	va_end (p);
}

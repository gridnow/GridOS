#include <types.h>
#include <stdarg.h>
#include <stdio.h>
#include <compiler.h>

#include "std_console.h"

DLLEXPORT int printf(const char *fmt, ...)
{
	char printf_buf[1024];
	va_list args;
	int printed;

	/* ºÏ³É */
	va_start(args, fmt);
	printed = vsnprintf(printf_buf, sizeof(printf_buf), fmt, args);
	va_end(args);

	/* To console */
	stdout_write(printf_buf, sizeof(printf_buf));

	return printed;
}

DLLEXPORT int puts(const char *s)
{
	return printf(s);
}

DLLEXPORT int putchar(int ch)
{	
	return printf("%c", ch);
}

DLLEXPORT int fputc(int s, FILE *stream)
{
	TODO("");
	return 0;
}

DLLEXPORT int fprintf(FILE *file, const char *format, ...)
{
	TODO("");
	return 0;
}

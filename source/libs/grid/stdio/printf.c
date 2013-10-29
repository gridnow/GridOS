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
	printed = vsprintf(printf_buf, fmt, args);
	va_end(args);

	/* To console */
	stdout_write(printf_buf, sizeof(printf_buf));

	return printed;
}

void printk(const char *fmt)
{
	printf(fmt);
}

DLLEXPORT int puts(const char *s)
{
	return printf(s);
}

/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL 调试相关
*/
#include <stdarg.h>
#include <debug.h>

#include <screen.h>
#include <console.h>

#if defined (__i386__)
extern struct hal_console_ops serial_console_ops;
#endif
static struct hal_console_ops *console_ops_array[]={
#if defined (__i386__)
	&serial_console_ops,
#endif
};
#define CONSOLE_COUNT (sizeof(console_ops_array) / sizeof(struct hal_console_ops*))

void console_write(char *string, int size)
{
	int i;

	/* Output to console driver */
	for (i = 0; i < CONSOLE_COUNT; i++)
		if(console_ops_array[i]->write)
			console_ops_array[i]->write(string, size);

}

#define MAX_PRINTF_SIZE 128
extern int vscnprintf(char *buf, int size, const char * fmt, va_list args);

int printk(const char *fmt, ...)
{
	va_list args;
	char printk_buf[MAX_PRINTF_SIZE];
	int printed_len;

	va_start(args,fmt);
	printed_len = vscnprintf(printk_buf, sizeof(printk_buf), fmt, args);
	va_end(args);
	printk_buf[printed_len] = '\0';										/* vscnprintf函数返回的值永远不会大于printk_buf数组的长度 */
	console_write(printk_buf, printed_len);

	return printed_len;
}

int printf(const char *fmt, ...)
{
	va_list args;
	char printk_buf[MAX_PRINTF_SIZE];
	int printed_len;

	va_start(args,fmt);
	printed_len = vscnprintf(printk_buf, sizeof(printk_buf), fmt, args);
	va_end(args);
	printk_buf[printed_len] = '\0';										/* vscnprintf函数返回的值永远不会大于printk_buf数组的长度 */
	console_write(printk_buf, printed_len);

	return printed_len;
}

int vprintf(const char *fmt, va_list args)
{
	char printk_buf[MAX_PRINTF_SIZE];
	int printed_len;

	printed_len = vscnprintf(printk_buf, sizeof(printk_buf), fmt, args);
	printk_buf[printed_len] = '\0';										/* vscnprintf函数返回的值永远不会大于printk_buf数组的长度 */
	console_write(printk_buf, printed_len);

	return printed_len;

}

int puts(const char * str)
{
	return printk(str);
}


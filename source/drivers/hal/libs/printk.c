/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL 调试相关
*/
#include <stdarg.h>
#include <debug.h>

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


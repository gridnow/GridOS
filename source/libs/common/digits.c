/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   wuxin
*   实现了数字到字符串转换的通用接口
*/

#include "include/digits.h"

static int digits_64(unsigned long long num, int base, int sign)
{
	int digits = 1;

	if (sign && ((long long) num < 0))
	{
		num = ((long long) num * -1);
		digits += 1;
	}

	while (num >= (unsigned long long) base)
	{
		digits += 1;
		num /= (unsigned long long) base;
	}

	return (digits);
}

static int digits_32(unsigned long long num, int base, int sign)
{
	int digits = 1;

	if (sign && ((int) num < 0))
	{
		num = ((int) num * -1);
		digits += 1;
	}

	while (num >= (unsigned int) base)
	{
		digits += 1;
		num /= (unsigned int) base;
	}

	return (digits);
}

int _digits(unsigned long long num, int base, int sign, int bits)
{
	if (base < 2)
		goto err;

	if (bits == 32)
		return digits_32((unsigned int)num, base, sign);
	if (bits == 64)
		return digits_64((unsigned long long)num, base, sign);
err:
	return -1;
}

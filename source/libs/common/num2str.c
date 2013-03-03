/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   wuxin
*   实现了数字到字符串转换的通用接口
*/

#include <types.h>
#include "include/digits.h"

void _lnum2str(unsigned long long num, char *string, int base, int sign)
{
	int digits = _digits(num, base, sign, 64);
	int char_count = 0;
	unsigned long long place = 1;
	unsigned long long rem = 0;
	int count;

	if (string == NULL)
	{
		return;
	}

	/* Negative? */
	if (sign && ((long long) num < 0))
	{
		string[char_count++] = '-';
		num = ((long long) num * -1);
		digits -= 1;
	}

	for (count = 0; count < (digits - 1); count ++)
		place *= (unsigned long long) base;

	while (place)
	{
		rem = (num % place);
		num = (num / place);

		if (num < 10)
			string[char_count++] = ('0' + num);
		else
			string[char_count++] = ('a' + (num - 10));
		num = rem;
		place /= (unsigned long long) base;
	}

	string[char_count] = '\0';

	return;
}

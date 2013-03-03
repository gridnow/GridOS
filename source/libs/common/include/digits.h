/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   wuxin
*   实现了数字到字符串转换的通用接口
*/
#ifndef COMMON_LIB_DIGITS_H
#define COMMON_LIB_DIGITS_H

/**
	@brief 得到一个数字到底需要多少个字符来表示
*/
int _digits(unsigned long long num, int base, int sign, int bits);

/**
	@brief 进行数字到字符串的转化
*/
void _lnum2str(unsigned long long num, char *string, int base, int sign);

#endif


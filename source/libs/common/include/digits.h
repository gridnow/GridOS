/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   wuxin
*   ʵ�������ֵ��ַ���ת����ͨ�ýӿ�
*/
#ifndef COMMON_LIB_DIGITS_H
#define COMMON_LIB_DIGITS_H

/**
	@brief �õ�һ�����ֵ�����Ҫ���ٸ��ַ�����ʾ
*/
int _digits(unsigned long long num, int base, int sign, int bits);

/**
	@brief �������ֵ��ַ�����ת��
*/
void _lnum2str(unsigned long long num, char *string, int base, int sign);

#endif


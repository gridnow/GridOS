/**
*  @defgroup String
*  @ingroup DDK
*
*  �����������������е��ַ�������ӿ�
*  @{
*/

#ifndef _DDK_STRING_H_
#define _DDK_STRING_H_

#include <string.h>

/**
	@brief ת��һ���ַ�����һ���޷��ų�����
*/
unsigned long simple_strtoul(const char *,char **,unsigned int);

/**
	@brief ת��һ���ַ�����һ���з��ų�����
*/
extern long simple_strtol(const char *,char **,unsigned int);
#endif

/** @} */
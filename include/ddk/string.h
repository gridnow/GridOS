/**
*  @defgroup String
*  @ingroup DDK
*
*  定义了驱动程序特有的字符串处理接口
*  @{
*/

#ifndef _DDK_STRING_H_
#define _DDK_STRING_H_

#include <string.h>

/**
	@brief 转换一个字符串到一个无符号长整型
*/
unsigned long simple_strtoul(const char *,char **,unsigned int);

/**
	@brief 转换一个字符串到一个有符号长整型
*/
extern long simple_strtol(const char *,char **,unsigned int);
#endif

/** @} */
/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*/

/**
*  @defgroup STRING
*  @ingroup 标准C库编程接口
*
*  标准字符串操作接口，头文件是string.h 
*
*  @{
*/
#ifndef _STRING_H
#define _STRING_H
#include <types.h>

BEGIN_C_DECLS;
/**
	@brief 将一缓冲区中的内容填充为特定内容

	在一段内存块中填充某个给定的值，它是对较大的结构体或数组进行清零操作的一种最快方法。

	@param[in] dst 目标缓冲区地址，用户层保证有有足够的size字节空间
	@param[in] ch 要填充的字符，虽然是int型的，但是只能当char类型的使用，这是业内规定
	@param[in] size 要写入多少字符

	@return
		dst的地址。
*/
void *memset(void *dst, int ch, unsigned long size);

/**
	@brief 拷贝一个缓冲区的内容到另外一个缓冲区

	从源src所指的内存地址的起始位置开始拷贝len个字节到目标dst所指的内存地址的起始位置中，
	一般用于内存拷贝，缓冲区拷贝等内存二进制操作的场景。它与字符串拷贝不一样，不会以内存中的内容
	是0来终止拷贝，而是用size参数来控制长度。
	另外，dst 与 src内存空间不可重叠。

	@param[in] dst 目标缓冲区，用户层保证有足够的内存保存src所指向的数据
	@param[in] src 源缓冲区
	@param[in] size 要拷贝的字符串字节长度

	@return
		目标传冲去地址dst。
*/
void *memcpy(void *dst, const void *src, unsigned long size);

/**
	@brief 拷贝字符串

	把从src地址开始且含有NULL结束符的字符串赋值到以dest开始的地址空间，
	src和dest所指内存区域不可以重叠且dest必须有足够的空间来容纳src的字符串。
	该函数终止的条件是在拷贝的字符串的过程中src字符串遇到了结束符'\0'。

	@param[in] dest 目标缓冲区地址，用户层保证有足够的内存保存src所指向的字符串
	@param[in] src  源缓冲区地址

	@return 目标串的地址。
*/
char *strcpy(char *__restrict dest, __const char *__restrict src);

/**
	@brief 从字符的头部到尾部搜索一个字符

	查找字符串中首次出现某字符的位置，从左向由搜索。

	@param[in] str 要搜索的字符串
	@param[in] ch  要超找的字符

	@return 返回字符首次出现的位置指针，如果字符串中不存在要搜索的字符则返回NULL。
*/
char * strchr(const char *str, int ch);

/**
	@brief 比较两个字符串的内容是否一样
	
	两个字符串自左向右逐个字符相比（按ASCII值大小相比较，即大小写敏感），直到出现不同的字符或遇'\0'为止。
	
	@param[in] s1 第一个字符串
	@param[in] s2 第二个字符串

	@return
		当s1<s2时，返回值<0；当s1=s2时，返回值=0；当s1>s2时，返回值>0。
*/
int strcmp(__const char *s1, __const char *s2);

/**
	@brief 在最大长度内比较两个字符串的内容是否一样
	
	两个字符串自左向右逐个字符相比（按ASCII值大小相比较，即大小写敏感），直到出现不同的字符或遇'\0'为止或者尝过了指定的最大长度。
	
	@param[in] s1 第一个字符串
	@param[in] s2 第二个字符串
	@param[in] count 要比较的最大长度。如果字符串的长度没有最大长度所描述的长，那么以字符串本身的最大长度为要比较的长度上限。如果该值为0，那么函数直接返回0。

	@return
		当s1<s2时，返回值<0；当s1=s2时，返回值=0；当s1>s2时，返回值>0。
*/
int strncmp(const char *s1, const char *s2, size_t count);

/**
	@brief 计算字符串的长度

	从一个字符串首地址开始扫描，直到碰到字符串结束符'\0'为止。

	@param[in] str 要统计长度的字符串首地址

	@return
		字符串字符长度，不包括'\0'在内。
*/
unsigned long strlen(const char * str);

/**
	@brief 计算字符串的长度

	从一个字符串首地址开始扫描，直到碰到字符串结束符'\0'为止，或者比对字节数达到了maxlen的上限。

	@param[in] str 要统计长度的字符串首地址
	@param[in] str 要统计的最大长度

	@return
		字符串字符长度，不包括'\0'在内。如果最大长度超过了字符串本身的长度，那么返回字符串的全长度，否则返回最大长度。
*/
size_t strnlen(const char *s, size_t maxlen);

/**
	@brief 比较内存区域的内容是否相同

	一个二进制比较函数，一个字节一个字节地比较两个缓冲区的内容是否相同。

	@param[in] cs 源缓冲区地址
	@param[in] ct 目标缓冲区地址
	@param[in] count 要比较的字符串

	@return 
		当cs<ct时，返回值<0；当cs=ct时，返回值=0；当cs>ct时，返回值>0。
*/
int memcmp(const void *cs, const void *ct, size_t count);

END_C_DECLS;

#endif

/** @} */


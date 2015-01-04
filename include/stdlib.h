/**
*  @defgroup STDLIB
*  @ingroup 标准C库编程接口
*
*  标准库操作接口，头文件是stdlib.h
*
*  @{
*/

#ifndef _STDLIB_H
#define _STDLIB_H

#include <types.h>

BEGIN_C_DECLS;

/**
	@brief 动态分配内存

	该函数返回类型为void*，表示未确定的类型指针。因为系统无法知道用户分配内存空间来存放什么类型的数据，所以返回void*，让用户按需进行类型转换。

	@param[in]	size	指定需要分配的内存空间大小，单位是字节

	@note	该函数在动态分配内存成功后，该内存空间的数据都是随机的。

	@return 分配成功返回指向已分配的内存空间起始地址，失败则返回NULL，并设置errno值。
*/
void *malloc(size_t size);

/**
	@brief 动态分配多个连续内存空间

	分配内存，并把内容清空为0。

	@param[in]	nmemb	连续内存空间个数
	@param[in]	size	每个内存空间大小，单位是字节

	@note	该函数在动态分配内存成功后，自动将该内存空间初始化为0。

	@return 分配成功返回指向已分配的内存空间起始地址，失败则返回NULL，并设置errno值。
*/
void *calloc(size_t nmemb, size_t size);

/**
	@brief 释放内存空间

	释放一片已经分配的内存，释放后的内存千万不能在被使用，否则产生不可预料的结果。

	@param[in]	ptr	指向要释放的内存空间

	@return 无返回值
*/
void free(void *ptr);

/**
	@brief	动态调整内存空间

	在已经分配的内存上再次调整其大小。

	@param[in]	ptr		指向需要调整大小的内存空间
	@param[in]	size	指定调整后的最终大小

	@note	动态调整的方法是先按size分配新内存空间，再将原内存空间数据按照size复制到新内存中，释放原内存。
			因为realloc的这一分配特点，所以该函数容易造成内存碎片。
	
	@return 成功返回指向已重新分配的内存空间起始地址，失败则返回NULL，并设置errno值。
*/
void *realloc(void *ptr, size_t size);

/**
	@brief 从环境中取字符串,获取环境变量的值

	取得参数envvar环境变量的内容。	参数envvar为环境变量的名称，如果该变量存在则会返回指向该内容的指针。环境变量的格式为envvar=value。
	getenv函数的返回值存储在一个全局二维数组里，当你再次使用getenv函数时不用担心会覆盖上次的调用结果。

	@param[in] envvar 要获取的环境变量的名称

	@return
		执行成功则返回指向该内容的指针，找不到符合的环境变量名称则返回NULL。如果变量存在但无关联值，它将运行成功并返回一个空字符串，即该字符的第一个字节是null。
*/
char *getenv(const char * envvar);

/**
	@brief 改变或增加环境变量

	修改一个进程在运行的过程中的一些环境变量，并不影响系统的环境变量，当进程退出后，所有的修改将全消失。
	
	@param[in] name 字符串变量名
	@param[in] value 字符串变量值
	@param[in] rewrite 当变量存在时控制是否替换现有值。如果为非0，那么替换现有值，否则如果已经有了个变量，值是不会被替换的

	@return
		返回值 执行成功则返回0，有错误发生时返回-1。
*/
int setenv(const char *name, const char *value, int rewrite);

/* TODO */
void abort(void);

END_C_DECLS;

#endif

/** @} */

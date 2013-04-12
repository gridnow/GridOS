/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*/

/**
*  @defgroup STDIO
*  @ingroup 标准C库编程接口
*
*  标准输入输出操作接口，头文件是stdio.h
*
*  @{
*/
#ifndef _STDIO_H
#define _STDIO_H

#include <types.h>
#include <stdarg.h>

BEGIN_C_DECLS;

typedef void* FILE;
extern FILE stderr;
extern FILE stdout;
extern FILE stdin;

/**
	@brief 把格式化的数据写入某个字符串缓冲区

	字串格式化命令，主要功能是把格式化的数据写入某个字符串中。
	该函数是个变参函数，使用时经常出问题，要注意缓冲区的长度是否能容下所有合成的字符。

	@param[in] buffer 指向欲写入的字符串地址
	@param[in] format 指向的内存里存放了格式字符串

	@note
		要合成的字符串超过缓冲区大小则会导致内存溢出，可考虑用snprintf。
	@return
		合成的字符串字节长度，不包括结束符。
*/
int sprintf (char *__restrict buffer, __const char *__restrict format, ...);

/**
	@brief 把格式化的数据写入某个字符串缓冲区

	功能同sprintf，但是该接口更安全，用户能指定合成的字符串最大尺寸，保证缓冲区不会溢出

	@param[in] buffer 指向欲写入的字符串地址
	@param[in] size 目标缓冲区的最大尺寸
	@param[in] format 指向的内存里存放了格式字符串

	@return
		合成的字符串字节长度，不包括结束符。如果返回的长度比给定的尺寸长，那么目标缓冲区中的内容将被裁减！
*/	
int snprintf(char *__restrict buffer, __const size_t size, const char *format, ...);

/**
	@brief 通过标准输出设备输出一组数据

	标准库函数vprintf函数与printf函数类似，所不同的是，它用一个参数取代了变长参数表，且此参数通过调用va_start宏进行初始化。
	
	@param[in] format 指向的内存里存放了格式字符串
	@param[in] arg 具体参数的可变列表

	@return
		输出的字符串字节长度。
*/
int vprintf(const char * format, va_list arg);

/**
	@brief 把格式化的数据写入某个文件

	功能大致与sprintf一直，但是输出的目标不再是某个一缓冲区，而是一个文件。

	@param[in] file 指向欲写入的文件对象
	@param[in] format 指向的内存里存放了格式字符串

	@return
		输出的字符串字节长度。
*/
int	fprintf(FILE * file, const char * format, ...);

/**
	@brief 通过标准输出设备输出一组数据

	转换、格式化并写 Argument 参数到标准输出。

	@param[in] format 指向的内存里存放了格式字符串

	@return
		输出的字符串字节长度。
*/
int printf(const char * format, ...);

/**
	@brief 通过标准输出设备输出一个字符串

	@param[in] str 指向的内存里存放了字符串

	@return
		输出的字符串字节长度。
*/
int puts(const char * str);

/**
	@brief 通过标准输出设备输出一个字符

	@param[in] ch 要输出的字符编码

	@return
		输出的字符字节长度。
*/
int putchar(int ch);

/**
	@brief 把格式化的数据写入某个字符串缓冲区

	字串格式化命令，主要功能是把格式化的数据写入某个字符串中。
	该函数是个变参函数，使用时经常出问题，要注意缓冲区的长度是否能容下所有合成的字符。

	@param[in] buffer 指向欲写入的字符串地址
	@param[in] format 指向的内存里存放了格式字符串
	@param[in] arg 具体参数的可变列表

	@note
		该函数容易出现内存溢出情况，建议使用vsnprintf。
	@return
		正常情况下返回生成字串的长度(除去\0)，错误情况返回负值。
*/
int vsprintf (char *__restrict buffer, __const char *__restrict format, va_list arg);

/**
	@brief 把格式化的数据写入某个字符串缓冲区

	字串格式化命令，主要功能是把格式化的数据写入某个字符串中。
	该函数是个变参函数，使用时经常出问题，要注意缓冲区的长度是否能容下所有合成的字符。

	@param[in] buffer 指向欲写入的字符串地址
	@param[in] format 指向的内存里存放了格式字符串
	@param[in] size 要拷贝的最大字节数
	@param[in] arg 具体参数的可变列表

	@return
		正常情况下返回生成字串的长度(除去\0)，错误情况返回负值。
*/
int vsnprintf(char *__restrict buffer, size_t size, const char *__restrict format, va_list args);

/**
	@brief	打开一个文件

	@param[in]	name	文件名
	@param[in]	mode	访问类型或打开方式，目前支持的类型如下：
			<ul>
				<li>r	打开只读文件，该文件必须存在；
				<li>r+	打开可读写文件，该文件必须存在；
				<li>w	打开只写文件，若文件存在则清除文件内容，即文件长度为0，
						若文件不存在则创建文件；
				<li>w+	打开可读写文件，若文件存在则清除文件内容，即文件长度为0，
						若文件不存在则创建文件；
				<li>a	附加方式打开只写文件，若文件存在则写入数据只会追加至文件尾，文件原数据无法修改，
						若文件不存在则创建文件；
				<li>a+	附加方式打开可读写文件，若文件存在则写入数据只会追加至文件尾，文件原数据无法修改，
						若文件不存在则创建文件。	
				<li>b	二进制流
			</ul>

	@return	文件打开成功返回执行该文件的文件指针，失败则返回NULL，并设置errno值。
*/
FILE * fopen(const char *name, const char *mode);

/**
	@brief	读取文件数据

	从文件中读取nmemb个数据元素，每个数据元素包括size字节。

	@param[in]	ptr		用于接收文件数据的内存地址，大小至少是size*nmemb字节
	@param[in]	size	数据元素大小，单位是字节
	@param[in]	nmemb	数据元素个数
	@param[in]	file	文件

	@return	读取成功返回实际读取的数据元素个数，失败则返回0，并设置errno值。
*/
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *file);

/**
	@brief	向文件写入数据

	向文件中写入nmemb个数据元素，每个数据元素包括size字节。
	写入行为与文件打开方式直接相关。

	@param[in]	ptr		用于存放输入数据的内存地址
	@param[in]	size	数据元素大小，单位是字节
	@param[in]	nmemb	数据元素个数
	@param[in]	file	文件

	@return	写入成功返回实际写入的数据元素个数，失败则返回0，并设置errno值。
*/
size_t fwrite(void *ptr, size_t size, size_t nmemb, FILE *file);

/**
	@brief	关闭文件

	@param[in]	file	文件

	@return	关闭成功返回0，否则返回-1。
*/
int fclose(FILE *file);

/**
	@brief	定位文件读取位置

	该函数设置文件内部索引的位置，从而设置访问文件的位置

	@param[in]	file	文件
	@param[in]	offset	相对文件访问位置的偏移量
	@param[in]	whence	访问位置三种模式：文件头SEEK_SET、文件尾SEEK_END和文件当前位置SEEK_CUR

	@return	定位成功返回0，失败则返回-1，并设置errno值。
*/
int fseek(FILE *file, long offset, int whence);

/**
	@brief	获取文件当前访问位置

	@param[in]	file	文件
	
	@return	成功返回文件当前访问位置，失败则返回-1，并设置errno值。
*/
long ftell(FILE *file);

/**
	@brief 从标准输入上获取一个按键

	一般一个键是1个字节，在用户按下键盘时该函数返回一个字节，但是
	posix 定义了某些键（比如方向键）是多字节的，因此当该函数得到的
	键值是特殊值的时候，有必要再次调用该函数得到剩下的键值以完整得到
	一个按键。

	@note
		具体键值参见POSIX标准。

	@return
		键值，如果没有按键则线程进入休眠状态。
*/
int getch();

/**
	@brief fseek位置取值
*/
#define SEEK_SET	0GridOS
#define SEEK_CUR	1
#define SEEK_END	2

END_C_DECLS;

#endif

/** @} */


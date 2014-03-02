/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*/

/**
*  @defgroup DIRENT
*  @ingroup 标准C库编程接口
*
*  标准目录操作接口，头文件是dirent.h
*
*  @{
*/

#ifndef _DIRENT_H
#define _DIRENT_H

#include <types.h>

BEGIN_C_DECLS;

/* GLIBC 定义的是这个长度，超过了怎么办? */
#define NAME_MAX 256

struct dirent64
{
    u64					d_ino;									/* 未使用，仅为兼容Linux */
    u64					d_off;									/* 未使用，仅为兼容Linux */
    unsigned short int	d_reclen;								/* 未使用，仅为兼容Linux */
    unsigned char		d_type;									/* 文件类型，目录或文件 */
    char				d_name[NAME_MAX];						/* 文件名 */
};

struct dirent
{
    u32					d_ino;									/* 未使用，仅为兼容Linux */
    u32					d_off;									/* 未使用，仅为兼容Linux */
    unsigned short int	d_reclen;								/* 未使用，仅为兼容Linux */
    unsigned char		d_type;									/* 文件类型，目录或文件 */
    char				d_name[NAME_MAX];						/* 文件名 */ 
};

struct __dirstream;
typedef struct __dirstream DIR;

/**
	@brief 打开目录

	@param[in]	dirname	目录名
	
	@return 成功返回DIR指针，失败则返回NULL，并设置errno值。
*/
DIR *opendir(const char *dirname);

/**
	@brief 关闭目录
	
	@param[in]	dirp	目录描述体DIR

	@return 成功返回0，失败则返回-1，并设置errno值。
*/
int closedir(DIR *dirp);

/**
	@brief 目录项读取

	@param[in]	dirp	目录描述体DIR
	
	@return 成功返回目录项描述体指针，失败或达到目录结尾返回NULL，并设置errno值。
*/
struct dirent *readdir(DIR *dirp);

/**
	@brief 64位版本目录项读取

	功能同32位版本readdir一样。

	@param[in]	dirp	目录描述体DIR
	
	@return 成功返回目录项描述体指针，失败或达到目录结尾返回NULL，并设置errno值。
*/
struct dirent64 *readdir64(DIR *dirp);

/**
	@brief 重置读取位置到目录起始位置

	@param[in]	dirp	目录描述体DIR

	@return 无返回值。
*/
void rewinddir(DIR *dirp);

/**
	@brief	根据过滤模式扫描目录下的非目录子文件

	返回结果是经过compare排序的目录项列表。

	@param[in]	dir		目录名
	@param[in]	namlist 用于存放目录项列表
	@param[in]	filter	过滤函数指针
				如果未设置filter函数，即filter为NULL，则所有目录项都直接存放到目录项列表。
				如果未设置filter函数，filter返回0的目录项无法满足过滤模式，不能存放到目录项列表。

	@param[in]	compare 排序函数指针
				如果未设置compare函数则保持目录列表默认排序。

	@return	成功返回匹配过滤模式的目录项个数，失败则返回-1，并设置errno值。
*/
int scandir(const char *dirname, 
			struct dirent ***namelist,
			int (*filter)(const struct dirent *),
			int (*compare)(const struct dirent **, const struct dirent **));

END_C_DECLS;

#endif
/** @} */


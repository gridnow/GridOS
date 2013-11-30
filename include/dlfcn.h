/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*/

/**
*  @defgroup dlfcn
*  @ingroup 标准C库编程接口
*
*  posix 兼容的动态库接口
*
*  @{
*/
#ifndef POSIX_DLFCN_H
#define POSIX_DLFCN_H

#include <compiler.h>

#define RTLD_LAZY			0x00001	/* Lazy function call binding.  */
#define RTLD_NOW			0x00002	/* Immediate function call binding.  */
#define	RTLD_BINDING_MASK   0x3	/* Mask of binding time value.  */
#define RTLD_NOLOAD			0x00004	/* Do not load the object.  */

#define RTLD_GLOBAL			0x00100
#define RTLD_LOCAL			0
#define RTLD_NODELETE		0x01000
typedef struct
{
	const char *dli_fname;	/* File name of defining object.  */
	void *dli_fbase;			/* Load address of that object.  */
	const char *dli_sname;	/* Name of nearest symbol.  */
	void *dli_saddr;			/* Exact value of nearest symbol.  */
} Dl_info;

/**
	@brief 打开共享动态库

	@param[in] file 有效动态库名, NULL 表示获取全局对象句柄
	@param[in] mode 动态库打开模式
	
	@return 
		NULL 失败，正确为动态库对象指针

	@note
		Posix: If the value of file is NULL, dlopen() shall provide a handle on a global symbol object.
*/
void *dlopen(const char *file, int mode);

/**	
	@brief 关闭动态
	
	把通过dlopen打开的动态库关闭，进程也不能再访问该动态库。

	@param[in] handle 动态库对象
	@return
		0 on success ,other is error	
*/
int dlclose(void *handle);

/**
	@brief 获得动态库的符号地址

	找到指定符号名在进程中的地址。

	@param[in] handle 动态库对象
	@param[in] name 符号名
	@return 
		符号地址，NULL则是失败
*/
void *dlsym(void *__restrict handle, __const char *__restrict name);

/**
	@brief 获一个有版本编号的符号地址
	
	找到带版本信息的符号名在进程中的地址。

	@param[in] handle 动态库对象
	@param[in] name 符号名
	@param[in] version 版本号

	@return 
		符号地址，NULL则是失败
*/
void *dlvsym(void *__restrict handle, __const char *__restrict name,
			  __const char *__restrict version);

/**
	@brief 返回动态库模块的错误信息

	When any of the above functions fails, call this function to return a string describing the error.  
	Each call resets the error string so that a following call returns null. 

	@return
		错误信息的字符串，或者NULL	
*/
char *dlerror (void);

/**
	@brief 通过地址找到符号
	
	通过一个地址找到它所对应最近的符号名字，并且返回到用户指定的info描述符。

	@param[in] address 要找的地址
	@param[in] info 返回结果的有效Dl_info描述符

	@return
		0 if the specified address can't be matched, or nonzero if it could be matched.
		If success,the info structure will be filled.
		If failed,the info->sname and info->sname will be set to NULL
*/
int dladdr(void * address, Dl_info * info);

#endif
/** @} */

/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*/

/**
*  @defgroup MemoryMapping
*  @ingroup 标准C库编程接口
*
*  Posix memory map
*
*  @{
*/

#ifndef POSIX_MMAN_H
#define POSIX_MMAN_H

#include <types.h>
#define PROT_READ		0x1				/* Page can be read */
#define PROT_WRITE		0x2				/* Page can be written */
#define PROT_EXEC		0x4				/* Page can be executed */
#define PROT_NONE		0x0				/* Page can not be accessed */

#define MAP_SHARED		0x01			/* Share changes. */
#define MAP_PRIVATE		0x02			/* Changes are private. */
#define MAP_FIXED		0x10			/* Interpret addr exactly. */
#define MAP_ANONYMOUS	0x20			/* Don't use a file. */
#define MAP_ANON		MAP_ANONYMOUS
#define MAP_FAILED		((void *) -1)

/**
	@brief 映射文件，可携带64位偏移

	@param[in] addr		指定映射区的起始地址，如果是NULL，则系统自动分配
	@param[in] len		设置地址长度，系统自动将其以页大小对其
	@param[in] prot		设置映射区的保护方式
	@param[in] flags	设置映射区的映射方式
	@param[in] fd		指定要被映射文件的文件描述符
	@param[in] offset	用户自定义偏移

	@return 成功返回映射区的地址，否则返回MAP_FAILED

*/
void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);

#endif

/** @} */

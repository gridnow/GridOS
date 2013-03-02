/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*/

/**
*  @defgroup MemoryMapping
*  @ingroup 标准C库编程接口
*
*  常量长度
*
*  @{
*/

#ifndef MMAN_H
#define MMAN_H

#define PROT_READ		0x1		/**< Page can be read */
#define PROT_WRITE		0x2		/**< Page can be written */
#define PROT_EXEC		0x4		/**< Page can be executed */
#define PROT_NONE		0x0		/**< Page can not be accessed */

#endif

/** @} */

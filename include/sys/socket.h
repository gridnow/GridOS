/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*/

/**
*  @defgroup Stddef
*  @ingroup 标准C库编程接口
*
*  标准定义
*  @{
*/

#ifndef SYS_SOCKET_H
#define SYS_SOCKET_H

#include <types.h>
struct sockaddr {
  u8 sa_len;
  u8 sa_family;
  char sa_data[14];
};

typedef u32 socklen_t;

#define AF_LOCAL 	1
#define AF_UNIX 	AF_LOCAL
#define	AF_INET		2		/* IP protocol family.  */

/* Socket type */
#define SOCK_STREAM 1

#endif

/** @} */

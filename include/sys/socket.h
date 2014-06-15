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

/** For compatibility with BSD code */
struct in_addr {
  u32 s_addr;
};


/* members are in network byte order */
struct sockaddr_in {
  u8 sin_len;
  u8 sin_family;
  u16 sin_port;
  struct in_addr sin_addr;
  char sin_zero[8];
};

typedef u32 socklen_t;

#define AF_LOCAL 	1
#define AF_UNIX 	AF_LOCAL
#define	AF_INET		2		/* IP protocol family.  */

/* Socket type */
#define SOCK_STREAM 1

#endif

/** @} */

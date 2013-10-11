#ifndef SESSION_AND_USER_MANAGER_H
#define SESSION_AND_USER_MANAGER_H

#include <compiler.h>
#include "sys/session_req.h"

/************************************************************************/
/* STDIO                                                                */
/************************************************************************/

/**
	@brief 每个Stdio驱动都应该有这些接口
*/
struct sau_stdio_interface
{
	size_t (*read)(struct ifi_package __user * input);
	size_t (*write)(unsigned char * __user buffer, size_t size);
};
int sau_read(struct ifi_package * input);
int sau_write(char * buffer, size_t size);

#define SESSION_REQUEST_MAX 10

#endif


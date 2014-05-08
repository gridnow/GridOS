#ifndef NET_SYS_REQ
#define NET_SYS_REQ

#ifndef _MSC_VER
#include <compiler.h>
#include <types.h>

#else
#include <Windows.h>
#endif

#include <kernel/kernel.h>
#include "syscall.h"

/************************************************************************/
/* SYSREQ                                                               */
/************************************************************************/

/**
	@brief Request package for stream managing
*/
struct sysreq_stream_manage
{
	/* INPUT */
	struct sysreq_common base;
	int ops;
#define SYSREQ_STREAM_OPS_CREATE 1
#define SYSREQ_STREAM_OPS_DELETE 2
	union __ops
	{
		/* 注册关心的事件 ,如果一个事件已经被监听，那么再次注册则替换回调函数  */
		struct __create_ops
		{
			int stream_id;
		} stream_create;
		struct __delete_ops
		{
			ke_handle stream;
		} stream_delete;
	} ops_private;
};

#endif

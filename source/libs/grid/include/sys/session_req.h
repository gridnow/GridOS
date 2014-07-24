#ifndef SESSION_SYS_REQ
#define SESSION_SYS_REQ

/* System call to session layer */
#include <ddk/input.h>
#include "syscall.h"



struct sysreq_session_stdio
{
	/* INPUT */
	struct sysreq_common base;
	int				rw;				/* 是读还是写 */	
	
	/* OUTPUT */
	union
	{
		/* 输入的数据格式，参见包的定义 */
		struct __input__
		{
			struct ifi_package pkg;
			int acture_pkg_count;
		}input;

		/* 输出的数据包 */
		struct __output__
		{
			char * buffer;
			size_t size;
		}output;
	}contents;
	int				result_code;
};

#endif /* Head */

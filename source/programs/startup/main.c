/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *
 *   系统初始化进程
 */

#include <kernel/ke_srv.h>

#include "../../libs/grid/include/sys/ke_req.h"

int test_bss_data;

/**
	@brief print the string without libc
 */
void early_print(char * string)
{
	struct sysreq_process_printf req = {0};
	req.base.req_id = SYS_REQ_KERNEL_PRINTF;
	req.string = string;
	system_call(&req);
}

void main()
{
	early_print("初始化进程启动中...\n");
	
}
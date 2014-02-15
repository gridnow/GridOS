/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

/**
	Session 和 会话管理器
	Wuxin (82828068@qq.com)
 */
#include <stdio.h>
#include <types.h>
#include <sys/session_req.h>
#include <sys/syscall.h>
#include "session.h"

/* Request handlers */
static void * interfaces[SESSION_REQUEST_MAX];

static unsigned long request_enqueue(unsigned long what, void *req)
{
	unsigned long (*function)(void * req) = interfaces[what];
	return function(req);
}

static struct ke_srv_info session_info = {
	.name				= "SESSION subsystem",
	.service_id_base	= SESSION_REQ_BASE,
	.request_enqueue	= request_enqueue,
};

static int stdio_write(struct sysreq_session_stdio * req)
{
	//printk("Calling stdio write. buffer %x..\n", req->contents.output.buffer);
	return sau_write(req->contents.output.buffer, 
		req->contents.output.size);
}

static int stdio_read(struct sysreq_session_stdio * req)
{
	int i = sau_read(&req->contents.input.pkg);	
// 	printk("pkg %x", req->contents.input.pkg.code);
	return i;
}

/*
	负责创建session层面的ifi设备.
	包括:键盘,鼠标等输入设备
*/
void session_create_input_device(void)
{
	extern int ifi_create_kb(void);
	//创建键盘设备
	ifi_create_kb();
}

DLLEXPORT bool session_init(int type)
{
	interfaces[SYS_REQ_SESSION_STDIO_READ - SESSION_REQ_BASE] = &stdio_read;
	interfaces[SYS_REQ_SESSION_STDIO_WRITE - SESSION_REQ_BASE] = &stdio_write;

	ke_srv_register(&session_info);	

	return 1;
}

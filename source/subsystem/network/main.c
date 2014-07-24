/**
	Network framework

	Sihai
*/
#include <types.h>
#include <stdio.h>
#include <list.h>
#include <string.h>

#include <compiler.h>
#include <ddk/net.h>
#include <kernel/ke_memory.h>
#include <kernel/ke_lock.h>
#include <kernel/ke_thread.h>

#include "sys/net_req.h"
#include "object.h"
//#include "fss.h"

struct nss_hwmgr *hwmgr;
static void *nss_dev = (void*)"eth0";

struct stream_control_block
{
	struct cl_object obj;
	
	struct list_head list;
	int stream_id;							// 协议中的标示符，比如tcp 的 port
	ke_thread thread;						// 所对应的流处理线程	

	ke_handle xchg_file;
};

static struct list_head stream_list;
static struct ke_spinlock stream_list_lock;
#define LOCK_STREAM_LIST() ke_spin_lock(&stream_list_lock);
#define UNLOCK_STREAM_LIST() ke_spin_unlock(&stream_list_lock);

static void dump_raw_package(void *buf, int size)
{
	unsigned char *p = buf;
	int off = 0;

	while (size > 0)
	{		
		if ((off % 16) == 0)
			printf("\n%08x: ", off);

		printf("%02x ", p[off]);
		off++;
		size--;
	}
}

static int input_stream(void *nss_dev, void *data, size_t size)
{
	//printf("网卡%s收到物理包，字节%ld.\n", nss_dev, size);
	//dump_raw_package(data, size);

	/* 根据协议类型找到数据包属于的进程 */
	//TODO:	

	/* 写入数据到文件，并唤醒等待数据的线程*/

	
	return 0;
}

static struct stream_control_block *create_stream(ke_thread thread, int stream_id)
{
	struct stream_control_block *stream;

	stream = km_valloc(sizeof(*stream));
	if (!stream)
		return NULL;
	memset(stream, 0, sizeof(*stream));
	stream->stream_id 	= stream_id;
	stream->thread 		= thread;
	stream->xchg_file 	= KE_INVALID_HANDLE;

	/* Open stream data exchanging file */
	TODO("目前是用/os/net/stream/0文件作为本流的交换文件");
	//fss_open("/os/net/stream/0");
	LOCK_STREAM_LIST();
	list_add_tail(&stream->list, &stream_list);
	UNLOCK_STREAM_LIST();

	return stream;
}

static void delete_stream(struct stream_control_block *stream)
{
	TODO("");
}

/************************************************************************/
/* Controller                                                           */
/************************************************************************/


/* Called now by DSS */
DLLEXPORT void nss_hwmgr_register(struct nss_hwmgr *mgr)
{
	hwmgr = mgr;

	printf("启动网络设备...\n");

	hwmgr->nops->read = input_stream;
	hwmgr->nops->open("eth0", nss_dev);
}

/************************************************************************/
/* Interface of this subsystem                                                           */
/************************************************************************/

/**
	@brief Stream 管理接口

	@return
		1,Create: the stream handle;
		2,Delete: return 0 for success;
		3,other, return -1UL;
*/
static unsigned long req_stream(struct sysreq_stream_manage *req)
{		
	ke_handle h_stream;
	struct stream_control_block *stream;
	
	switch (req->ops)
	{
		case SYSREQ_STREAM_OPS_CREATE:
		{
			stream = create_stream(ke_current(), req->ops_private.stream_create.stream_id);
			if (!stream)
				goto create_err0;
			h_stream = ke_handle_create(stream);
			if (h_stream == KE_INVALID_HANDLE)
				goto create_err1;
			return h_stream;
create_err1:
			delete_stream(stream);			
create_err0:			
			return KE_INVALID_HANDLE;
		}
		case SYSREQ_STREAM_OPS_DELETE:
		{
			stream = ke_handle_translate(req->ops_private.stream_delete.stream);
			if (!stream)
				goto delete_err0;
			ke_handle_and_object_destory(req->ops_private.stream_delete.stream, stream);
delete_err0:
			return -1UL;
		}	
	}
	
	return -1UL;
}

static void * interfaces[SYS_REQ_NETWORK_MAX];
static unsigned long kernel_srv(unsigned long req_id, void *req)
{
	unsigned long (*func)(void * req) = interfaces[req_id];
	return func(req);
}

const static struct ke_srv_info ke_srv_nss = {
	.name = "NSS服务",
	.service_id_base = SYS_REQ_NETWORK_BASE,
	.request_enqueue = kernel_srv,
};

void nss_main()
{
	int i;
	
	INIT_LIST_HEAD(&stream_list);
	ke_spin_init(&stream_list_lock);
	hwmgr = NULL;
	
	for(i = 0; i < SYS_REQ_NETWORK_MAX; ++i)
		interfaces[i] = ke_srv_null_sysxcal;
	interfaces[SYS_REQ_NETWORK_STREAM - SYS_REQ_NETWORK_BASE] = req_stream;
	
	ke_srv_register(&ke_srv_nss);
}

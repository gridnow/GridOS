/*
	MyTCPIP
	
	Sihai
*/

#include <ystd.h>
#include <pthread.h>
#include <string.h>

#include <ddk/grid.h>

/* The Stack */
#include "netif.h" 
#include "etharp.h"
#include "pbuf.h"
#include "init.h"

#define DEFAULT_STREAM_FILE_PATH "/os/net/stream"
#define DEFAULT_MAX_PKG_SIZE	2048

struct net_interface
{
	/* About worker */
	pthread_t worker;

	/* About netif */
	struct netif netif;

	/* Counter */
	unsigned long dropped;
};

/* 一次输入的上下文 */
struct stream_input_ctx
{
	struct net_interface *pni;	
	y_handle stream_file;	
	char stream_file_name[128];
};

static struct net_interface global_net_interface;

static int do_connect()
{
	TODO("");

	return -ENOSYS;
}

static struct grid_netproto ip_netproto = {
	.proto_name = "IPv4 grid_netproto",
	.connect = do_connect,
};

static void setup_stream_file(struct stream_input_ctx *ctx)
{
	// TODO: Tell system our pid 
	sprintf(ctx->stream_file_name, "%s/%d", 
		DEFAULT_STREAM_FILE_PATH, 0/*TODO:get pid*/);
}

static void stream_input(struct y_message *msg)
{
	struct pbuf *pb;
	struct stream_input_ctx *ctx;
	size_t size = DEFAULT_MAX_PKG_SIZE;

	y_message_read(msg, &ctx);

	if (NULL == (pb = pbuf_alloc(PBUF_RAW, size, PBUF_RAM)))
		goto err;

	/* 
		从消息中得到流文件，并读取流文件。
		此处可以用内存影射的方式替代read，从而
		减少一次内存拷贝。
	*/
	y_file_read(ctx->stream_file, pb->payload, size);
	ethernet_input(pb, &ctx->pni->netif);

	/* 流文件中还有数据吗? */
	//TODO: 轮询流文件
	
	return;

err:
	ctx->pni->dropped++;
}

static err_t stream_output(struct netif *netif, struct pbuf *p)
{
	struct pbuf *q;

	for(q = p; q != NULL; q = q->next) 
	{
   		 /* Send the data from the pbuf to the interface, one pbuf at a
			time. The size of the data in each pbuf is kept in the ->len
			variable. 
			send data from(q->payload, q->len);
		*/
		printf("TODO:发送数据包 %d 字节.\n", q->len);
	}
	
	return ERR_OK;
}

static void *stream_input_worker(void *parameter)
{
	struct net_interface *pni = parameter;
	struct stream_input_ctx ctx;

	ctx.pni = pni;	
	setup_stream_file(&ctx);
	
	if (Y_INVALID_HANDLE == (ctx.stream_file = y_file_open(ctx.stream_file_name)))
	{
		printf("打开网络流文件 %s 失败.\n", ctx.stream_file_name);
		goto err0;
	}
	if (y_file_event_register(ctx.stream_file, Y_FILE_EVENT_WRITE, stream_input, &ctx) < 0)
		goto err1;
	printf("Waiting network input...\n");
	y_message_loop();

err1:
	y_file_close(ctx.stream_file);
err0:		
	return NULL;
}

static int nif_init(struct net_interface *pni)
{
	struct netif *nif		= &pni->netif;

	nif->hwaddr_len			= ETHARP_HWADDR_LEN;
	nif->mtu				= 1500;
	nif->flags				= NETIF_FLAG_ETHERNET | NETIF_FLAG_ETHARP;
	nif->linkoutput			= stream_output;
	nif->output				= etharp_output;

	if (pthread_create(&pni->worker, NULL, stream_input_worker, pni))
		goto err;

	return ERR_OK;
	
err:
	return ERR_MEM;
}

int dll_main(void)
{
	/* Core system */
	lwip_init();

	/* Virtual network interface */
	nif_init(&global_net_interface);

	return 0;
}

unsigned int sys_now(void)
{
	printf("sys_now not implemented.\n");
	return 0;
}

DLLEXPORT struct grid_netproto *grid_acquire_netproto()
{
	return &ip_netproto;
}

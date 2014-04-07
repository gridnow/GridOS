/*
	MyTCPIP
	
	Sihai
*/

#include <ystd.h>
#include <pthread.h>
#include <string.h>

#include <ddk/grid.h>
#include <ddk/net.h>

#include <ring_buff.h>
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
	
	/* Buffer */
	void *stream_map;
	
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

static bool setup_stream_file(struct stream_input_ctx *ctx)
{
	// TODO: Tell system our pid 
	sprintf(ctx->stream_file_name, "%s/%d", 
		DEFAULT_STREAM_FILE_PATH, 0/*TODO:get pid*/);
	
	if (Y_INVALID_HANDLE == (ctx->stream_file = y_file_open(ctx->stream_file_name, Y_FILE_OPERATION_NOCACHE)))
	{
		printf("打开网络流文件 %s 失败.\n", ctx->stream_file_name);
		goto err0;
	}

	if (NULL == (ctx->stream_map = y_file_mmap(ctx->stream_file, 0, KM_PROT_READ | KM_PROT_WRITE, 0, 0)))
	{
		printf("影射网络文件失败。\n");
		goto err1;
	}

	return true;

err1:
	y_file_close(ctx->stream_file);
err0:
	return false;
}

static void close_stream_file(struct stream_input_ctx *ctx)
{
	if (ctx->stream_map)
	{
		TODO("反隐射ctx->stream_map");
		ctx->stream_map = NULL;
	}
	y_file_close(ctx->stream_file);
	ctx->stream_file = Y_INVALID_HANDLE;
}

static void stream_input(struct y_message *msg)
{
	struct ring_package *ring_pkt;
	struct ring_buff_cache *cache;
	struct pbuf *pb;
	struct stream_input_ctx *ctx;
	size_t size = DEFAULT_MAX_PKG_SIZE;
	
	printf("Stream got input....");
	y_message_read(msg, &ctx);

	/* 
		从消息中得到流文件，并读取流文件。
		此处可以用内存影射的方式替代read，从而
		减少一次内存拷贝。
	*/

	cache = (struct ring_buff_cache *)(ctx->stream_map);

	/* 获取可读报文 */
	while (ring_pkt = ring_cache_read_package(cache))
	{
		if (NULL == (pb = pbuf_alloc(PBUF_RAW, size, PBUF_REF)))
			goto err;

		pb->payload = (void *)cache + ring_pkt->package_offset;
		ethernet_input(pb, &ctx->pni->netif);
	}

	//TODO: 轮询流文件
	printf("OK.");
	
	return;
	
err:
	printf("pbuf 分配失败\n");
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

static void nif_startup(struct netif *nif)
{
	ip_addr_t test_ipaddr, test_netmask, test_gw;
		
	/* Setup IP */
	// TODO: ip and mac should get from system infomation center
	IP4_ADDR(&test_gw, 10,137,36,200);
	IP4_ADDR(&test_ipaddr,	10,137,36,15);
	IP4_ADDR(&test_netmask, 255,255,255,0); 	
	nif->hwaddr[0] = 0x11;
	nif->hwaddr[1] = 0x22;
	nif->hwaddr[2] = 0x33;
	nif->hwaddr[3] = 0x44;
	nif->hwaddr[4] = 0x55;
	nif->hwaddr[5] = 0x66;	
	netif_set_ipaddr(nif, &test_ipaddr);
	netif_set_netmask(nif, &test_netmask);
	netif_set_gw(nif, &test_gw);
		
	netif_set_up(nif);
}		

static void *stream_input_worker(void *parameter)
{
	struct net_interface *pni = parameter;
	struct stream_input_ctx ctx;
	
	/* Startup netif */	
	nif_startup(&pni->netif);
	
	/* Setup stream */
	ctx.pni = pni;	
	if (false == setup_stream_file(&ctx))
		goto err0;
	if (y_file_event_register(ctx.stream_file, Y_FILE_EVENT_READ, stream_input, &ctx) < 0)
		goto err1;

	/* Wait stream */
	y_message_loop();
err1:
	close_stream_file(&ctx);
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

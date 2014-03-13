/*
	MyTCPIP
	
	Sihai
*/

#include <ystd.h>
#include <pthread.h>
#include <string.h>

#include <ddk/grid.h>

#include "netif.h" 
#include "etharp.h"
#include "pbuf.h"
#include "init.h"

#define DEFAULT_STREAM_FILE_PATH "/os/net/stream"

struct net_interface
{
	/* About worker */
	pthread_t worker;
	y_handle stream_file;
	char stream_file_name[128];

	/* About netif */
	struct netif netif;

	/* Counter */
	unsigned long dropped;
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

static void setup_stream_file(struct net_interface *pni)
{
	// TODO: Tell system our pid 
	sprintf(pni->stream_file_name, "%s/%d", 
		DEFAULT_STREAM_FILE_PATH, 0/*TODO:get pid*/);
}

static void stream_input(struct net_interface *pni)
{
	void *data;
	size_t size;
	struct pbuf *pb;

	/* Read data file stream file */
	// TODO:
	size = 0;
	data = NULL;

	if (NULL == (pb = pbuf_alloc(PBUF_RAW, size, PBUF_RAM)))
		goto err;

	// TODO: 采用0拷贝方法
	memcpy(pb->payload, data, size);
	ethernet_input(pb, &pni->netif);

	return;

err:
	pni->dropped++;
}

static void *stream_input_worker(void *parameter)
{	
	struct net_interface *pni = parameter;
	
	/* 监听Stream文件 */
	setup_stream_file(pni);
	if (Y_INVALID_HANDLE == (pni->stream_file = y_file_open(pni->stream_file_name)))
	{
		printf("打开网络流文件 %s 失败.\n", pni->stream_file_name);
		goto err0;
	}
	if (y_file_event_register(pni->stream_file, Y_FILE_EVENT_WRITE, stream_input, pni) < 0)
		goto err1;
		
	y_message_loop();

err1:
	y_file_close(pni->stream_file);
err0:		
	return NULL;
}

static int nif_init(struct net_interface *pni)
{
	struct netif *nif		= &pni->netif;

	nif->hwaddr_len			= ETHARP_HWADDR_LEN;
	nif->mtu				= 1500;
	nif->flags				= NETIF_FLAG_ETHERNET | NETIF_FLAG_ETHARP;
//	nif->driver_ops			= &user_if_ops;
//	nif->linkoutput			= user_if_send;
//	nif->output				= etharp_output;

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

DLLEXPORT struct grid_netproto *grid_acquire_netproto()
{
	return &ip_netproto;
}

unsigned int sys_now(void)
{
	printf("sys_now not implemented.\n");
	return 0;
}

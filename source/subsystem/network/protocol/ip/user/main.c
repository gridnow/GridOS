/*
	MyTCPIP
	
	Sihai
*/

#include <stdio.h>
#include <ystd.h>
#include <pthread.h>

#include "netif.h" 
#include "etharp.h"
#include "pbuf.h"
#include "init.h"

struct net_interface
{
	/* About worker */
	pthread_t worker;
	y_handle stream_file;
	char stream_file_name[128];

	/* About netif */
	struct netif netif;
};
struct net_interface global_net_interface;

static void stream_input(struct net_interface *pni)
{
	//default_bk_feedback(netif, raw_package, data_desc, size);
}

static void *stream_input_worker(void *parameter)
{	
	struct net_interface *pni = parameter;
	
	/* ¼àÌýStreamÎÄ¼þ */
	if (Y_INVALID_HANDLE == (pni->stream_file = y_file_open(pni->stream_file_name)))
		goto err0;
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

int main(void)
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

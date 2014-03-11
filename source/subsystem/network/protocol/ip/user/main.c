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

struct net_interface
{
	pthread_t worker;
	y_handle stream_file;
};

static void stream_input(struct net_interface *nip)
{
	//default_bk_feedback(netif, raw_package, data_desc, size);
}

static void *stream_input_worker(void *parameter)
{	
	struct net_interface *nip = parameter;
	
	/* ¼àÌýStreamÎÄ¼þ */
	if (Y_INVALID_HANDLE == (nip->stream_file = y_file_open(nip->stream_file_name)))
		goto err0;
	if (y_file_event_register(nip->stream_file, Y_FILE_EVENT_WRITE, stream_input, nip) < 0)
		goto err1;
		
	y_message_loop();

err1:
	y_file_close(nip->stream_file);
err0:		
	return NULL;
}

static int nif_init(struct netif *nif, struct netif_init_info *info)
{
	struct net_interface *p;

	if ((p = calloc(1, sizeof(*p))) == NULL)
		goto err;
	nif->core_count			= info->core_count;
	nif->core_id			= info->core_id;
	nif->hwaddr_len			= ETHARP_HWADDR_LEN;
	nif->mtu				= 1500;
	nif->flags				= NETIF_FLAG_ETHERNET | NETIF_FLAG_ETHARP;
	nif->driver_object		= p;
//	nif->driver_ops			= &user_if_ops;
//	nif->linkoutput			= user_if_send;
//	nif->output				= etharp_output;

	if (pthread_create(&p->worker, NULL, stream_input_worker, nif))
		goto err;

	return ERR_OK;
	
err:
	if (p)
		free(p);
	return ERR_MEM;
}

static bool create_net_interface()
{
	struct netif_init_info para;
	struct netif *nif;

	memset(&para, 0, sizeof(para));
	para.type = NETIF_ETHERNET;
	nif = netif_create(nif_init, &para);

	return nif;
}

int main(void)
{
	create_net_interface();

	return 0;
}

/*
	MyTCPIP
	
	Sihai
*/

#include <stdio.h>
#include <pthread.h>

#include "core/netif.h" 
#include "core/etherarp.h"
#include "core/pbuf.h"


pthread_key_t netif_key = (pthread_key_t)-1;

struct user_if
{
	pthread_t worker;
};

static struct netif_driver_ops user_if_ops;

void dump_raw_package(void *buf, int size)
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

/**
	@brief The hardware layer to free zerocopy buffer 
*/
void mytcpip_free_zerocopy_object(void *p)
{

}

static void init_if(struct netif *nif)
{
	ip_addr_t test_ipaddr, test_netmask, test_gw;

	/* TODO: 从系统获取IP + MAC */
	IP4_ADDR(&test_gw, 192,168,1,1);
	IP4_ADDR(&test_ipaddr, 192,168,1,12);
	IP4_ADDR(&test_netmask, 255,255,255,0);
	nif->hwaddr[0] = 0x11;
	nif->hwaddr[1] = 0x22;
	nif->hwaddr[2] = 0x33;
	nif->hwaddr[3] = 0x44;
	nif->hwaddr[4] = 0x55;
	nif->hwaddr[5] = 0x66;
	
	netif_set_address(nif, &test_ipaddr, &test_netmask, &test_gw);	
	myip_instance_init(nif);
}

static void create_new_stream()
{
	y_handle stream;
	
	/* 注册进程socket目录监听事件 */
	stream_dir = opendir(path_of_process_net_stream);
	if (stream_dir == Y_INVALID_HANDLE)
		goto err;
	y_file_event_register(stream_dir, Y_FILE_EVENT_CREATE_FILE, create_new_stream, NULL);
	
}

static void *user_if_worker(void *parameter)
{
	y_handle stream_dir;
	struct netif *netif;
	
	/* This thread use its private netif */
	netif = parameter;
	pthread_setspecific(netif_key, netif);
	init_if(netif);

	/* 注册进程socket目录监听事件 */
	stream_dir = opendir(path_of_process_net_stream);
	if (stream_dir == Y_INVALID_HANDLE)
		goto err;
	y_file_event_register(fpoll_info, Y_FILE_EVENT_CREATE_FILE, stream_dir, create_new_stream, NULL);
		
	y_message_loop();
	
	/* Handle the packages */
	//default_bk_feedback(netif, raw_package, data_desc, size);
	
	return NULL;
}

static err_t user_if_send(struct netif *netif, struct pbuf *p)
{
	//TODO
	return ERR_OK;
}


static int user_if_init(struct netif *nif, struct netif_init_info *info)
{
	struct user_if *p;

	if ((p = calloc(1, sizeof(*p))) == NULL)
		goto err;
	nif->core_count			= info->core_count;
	nif->core_id			= info->core_id;
	nif->hwaddr_len			= ETHARP_HWADDR_LEN;
	nif->mtu				= 1500;
	nif->flags				= NETIF_FLAG_ETHERNET | NETIF_FLAG_ETHARP;
	nif->driver_object		= p;
	nif->driver_ops			= &user_if_ops;
	nif->linkoutput			= user_if_send;
	nif->output				= etharp_output;

	if (pthread_create(&p->worker, NULL, user_if_worker, nif))
		goto err;

	return ERR_OK;
	
err:
	if (p)
		free(p);
	return ERR_MEM;
}

struct netif *user_if_add()
{
	struct netif_init_info para;
	struct netif *nif;

	memset(&para, 0, sizeof(para));
	para.type		= NETIF_ETHERNET;
	nif = netif_create(user_if_init, &para);

	return nif;
}

int main(void)
{
	pthread_key_create(&netif_key, NULL);
	myip_init();	
	user_if_add();

	return 0;
}

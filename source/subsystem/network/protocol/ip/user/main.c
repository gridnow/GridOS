/*
	MyTCPIP
	
	Sihai
*/

#include <ystd.h>
#include <pthread.h>
#include <string.h>
#include <message.h>
#include <socket.h>

#include <ddk/grid.h>
#include <ddk/net.h>

#include <ring_buff.h>
/* The Stack */
#include "netif.h" 
#include "etharp.h"
#include "pbuf.h"
#include "init.h"
#include "tcp.h"


/* For testing */
#define USE_MAP_LOOP_BACK 1

#define DEFAULT_STREAM_FILE_PATH "/os/net/stream"

#define DEFAULT_ACCEPT_LEN   4

#define NET_CONNET_IS_BLOCK(netconn) ((netconn->flag) & NET_F_BLOCK)

/* netconn lock */
#define GRD_NETCONN_LOCK_INIT(netconn)
#define GRD_NETCONN_LOCK(netconn) 
#define GRD_NETCONN_UNLOCK(netconn)

/* Message request */
#define SOCKET_MANAGE_MSG_MAKE_OPS(ops, add_info) \
	((ops) | ((add_info) << 16))
#define SOCKET_MANAGE_MSG_GET_OPS(ops_code) \
	((ops_code) & 0xffff)
#define SOCKET_MANAGE_MSG_GET_ADD(ops_code) \
	(((ops_code) >> 16) & 0xffff)

#define connect_msg_id					1
#define bind_msg_id						2
#define create_delete_socket_msg_id		3
	#define SOCKET_MSG_ID_SUB_OPS_CREATE	1
	#define SOCKET_MSG_ID_SUB_OPS_DELETE	2
#define listen_msg_id					4

#define send_connect_msg_to_ip_thread(netconn, sockaddr) ({ \
	int __ret; \
	/* 构造发送消息 */ \
	MSG_MAKE(pmsg, MSG_FLAGS_SYNC, 2, connect_msg_id) \
	MSG_MAKE_OUT_PARA(pmsg, pdata) \
	pdata[0] = (MSG_DATA_TYPE)(netconn); \
	pdata[1] = (MSG_DATA_TYPE)(sockaddr); \
	y_message_send(global_net_interface.worker, pmsg); \
	__ret = pdata[0]; \
	__ret; \
})

#define send_bind_msg_to_ip_thread(netconn, sockaddr) ({ \
	int __ret; \
	/* 构造发送消息 */ \
	MSG_MAKE(pmsg, MSG_FLAGS_SYNC, 2, bind_msg_id) \
	MSG_MAKE_OUT_PARA(pmsg, pdata) \
	pdata[0] = (MSG_DATA_TYPE)(netconn); \
	pdata[1] = (MSG_DATA_TYPE)(sockaddr); \
	y_message_send(global_net_interface.worker, pmsg); \
	__ret = pdata[0]; \
	__ret; \
 })
#define send_socket_manage_msg_to_ip_thread(ops, para, err_code) ({ \
	void* __ret; \
	/* 构造发送消息 */ \
	MSG_MAKE(pmsg, MSG_FLAGS_SYNC, 2, create_delete_socket_msg_id) \
	MSG_MAKE_OUT_PARA(pmsg, pdata) \
	pdata[0] = (MSG_DATA_TYPE)(ops); \
	pdata[1] = (MSG_DATA_TYPE)(para); \
	y_message_send(global_net_interface.worker, pmsg); \
	__ret = (void*)pdata[1]; \
	err_code = (int)pdata[0]; \
	__ret; \
})
#define send_listen_msg_to_ip_thread(netconn, backlog) ({ \
	int __ret; \
	/* 构造发送消息 */ \
	MSG_MAKE(pmsg, MSG_FLAGS_SYNC, 2, listen_msg_id) \
	MSG_MAKE_OUT_PARA(pmsg, pdata) \
	pdata[0] = (MSG_DATA_TYPE)(netconn); \
	pdata[1] = (MSG_DATA_TYPE)(backlog); \
	y_message_send(global_net_interface.worker, pmsg); \
	__ret = pdata[0]; \
	__ret; \
})

struct net_interface
{
	/* About worker */
	pthread_t worker;

	/* About netif */
	struct netif netif;
	
	/* Counter */
	unsigned long dropped;
	
	/* Input contex */
	y_handle stream_file;
	
	/* Buffer */
	void *stream_map;
	
	char stream_file_name[128];
};

static struct net_interface global_net_interface;
static int wait_times = 5000;

static int stack_err_to_posix_err(int ret)
{
	/* OK? Nothing to translate */
	if (!ret) return 0;
	
	/* TODO: 详细转换... */
	return -EFAULT;
}

/*********************************************************************
 Socket API Logic
*********************************************************************/

static int delete_pcb(void *pcb, int type)
{
	int ret;
	
	/* Just tell him..... */
	send_socket_manage_msg_to_ip_thread(SOCKET_MANAGE_MSG_MAKE_OPS(SOCKET_MSG_ID_SUB_OPS_DELETE, type), pcb, ret);
	
	return ret;
}

static void *netconn_init(struct grd_netconn *netconn, void *pcb, int type)
{
	/* Init the connection object */
	GRD_NETCONN_LOCK_INIT(netconn);
	INIT_LIST_HEAD(&netconn->accep_queue);
	netconn->protocal_control_block = pcb;
	netconn->net_types				= type;
	
	if (Y_INVALID_HANDLE == (netconn->event = y_event_create(false, false)))
		goto err1;
	return netconn;

err1:
	return NULL;
}

/**
	@brief socket create API
*/
static void *grd_socket(int type, int proto)
{
	void *pcb;
	int ret;
	struct grd_netconn *netconn;
	
	/* Valid socket type?, because we will us type in SOCKET_MANAGE_MSG_MAKE_OPS */
	if (type & 0xffff0000)
		goto err;
	
	if (NULL == (netconn = (struct grd_netconn *)malloc(sizeof(*netconn))))
		goto err;
	if (NULL == (pcb = send_socket_manage_msg_to_ip_thread(SOCKET_MANAGE_MSG_MAKE_OPS(SOCKET_MSG_ID_SUB_OPS_CREATE, type), netconn, ret)))
		goto err1;
	if (ret)
	{
		printf("Error code = %d.\n", ret);
	}
	
	if (NULL == netconn_init(netconn, pcb, type))
		goto err2;
	

	return netconn;
	
err2:
	delete_pcb(pcb, type);
err1:
	free(netconn);	
err:
	return NULL;
}

static err_t tcp_recved_fn(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	return ERR_OK;
}

static err_t tcp_send_fn(void *arg, struct tcp_pcb *pcb, u16_t len)
{
	return ERR_OK;
}

static void tcp_err_call(void *arg, err_t err)
{
	return;
}

/**
	@brief 初始化协议栈回调函数
 */
static void setup_tcp_fn(struct tcp_pcb *pcb)
{
	tcp_recv(pcb, tcp_recved_fn);
    tcp_sent(pcb, tcp_send_fn);
	tcp_err(pcb, tcp_err_call);
}

/**
	@brief 协议栈链接成功的回调, 协议栈线程使用
*/
static err_t tcp_connected(void *arg, struct tcp_pcb *pcb, err_t err)
{
	struct grd_netconn *netconn = (struct grd_netconn *)arg;
	
	printf("Tcp 链接成功，唤醒等待者链接完成的线程...\n");
	setup_tcp_fn(pcb);
	y_event_set(netconn->event);
	return ERR_OK;
}

/**
	@brief Connect API
*/
static int grd_connect(struct grd_netconn *netconn, const void *addr, size_t addr_len)
{
	int ret = 0;
	y_wait_result wait;
	
	if ((ret = send_connect_msg_to_ip_thread(netconn, addr)))
		goto end;
	
	/* Ok, let's wait the result */
	wait = y_event_wait(netconn->event, wait_times);
	if (wait != KE_WAIT_OK)
	{
		if (wait == KE_WAIT_TIMEDOUT)
			ret = -ETIMEDOUT;
		else
			ret = -EFAULT;
	}
end:
	return ret;
}

/**
	@brief bind socket
*/
static int grd_bind(struct grd_netconn *netconn, void *addr, size_t addr_len)
{
	return send_bind_msg_to_ip_thread(netconn, addr);
}

/**
	@brief 接受回调函数
*/
static err_t tcp_acceptted(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	struct grd_netconn *netconn = (struct grd_netconn *)arg;
	struct accept_queue *accept;

	if (!(accept = (struct accept_queue *)malloc(sizeof(*accept))))
	{
		/* Returning error will cause the stack to delete this connection */
		/* See the calling of TCP_EVENT_ACCEPT */
		return ERR_BUF;
	}
	
	memset(accept, 0, sizeof(*accept));
	/* init accept and add to netconn accept list */
	accept->accpeted_protocal_control_block = newpcb;
	setup_tcp_fn(newpcb);
	
	/* lock netconn and add list head */
	GRD_NETCONN_LOCK(netconn);	
	list_add_tail(&accept->list, &netconn->accep_queue);
	y_event_set(netconn->event);
	GRD_NETCONN_UNLOCK(netconn);

	return ERR_OK;
}

/**
	@brief listen API
*/
static int grd_listen(struct grd_netconn *netconn, int backlog)
{
	backlog = backlog ? backlog : DEFAULT_ACCEPT_LEN;

	return send_listen_msg_to_ip_thread(netconn, backlog);
}

/**
	@brief accept API
 
	Create a new connect;
*/
static void *grd_accept(struct grd_netconn *netconn, void *addr_out, size_t *addr_len)
{
	struct tcp_pcb *newpcb;
	struct accept_queue *old;
	struct grd_netconn *new_netconn;
	
again:
	/* lock */
	GRD_NETCONN_LOCK(netconn);

	/* empty queue? */
	if (list_empty(&netconn->accep_queue))
	{
		GRD_NETCONN_UNLOCK(netconn);
		goto wait_accept;
	}

	old = list_entry(netconn->accep_queue.next, struct accept_queue, list);
	/* alloc new netconn and set to new_file */
	newpcb = old->accpeted_protocal_control_block;

	/* set old struct accept queue for free */
	list_del(&old->list);
	free(old);
	GRD_NETCONN_UNLOCK(netconn);
	
	/* 
		Now we are ready to create a new connection
		A bit like socket creating, see grd_socket.
	 */
	if (NULL == (new_netconn = (struct grd_netconn *)malloc(sizeof(*new_netconn))))
		goto err;
	if (netconn_init(new_netconn, newpcb, SOCK_STREAM) == NULL)
		goto err1;

	return new_netconn;
	
wait_accept:
	y_event_wait(netconn->event, wait_times);
	goto again;
err1:
	delete_pcb(newpcb, SOCK_STREAM);
err:
	return NULL;
}

DLLEXPORT struct grid_netproto grid_acquire_netproto = {
	.proto_name = "IPv4 grid_netproto",
	.socket  = grd_socket,
	.connect = grd_connect,
	.bind    = grd_bind,
	.listen  = grd_listen,
	.accept  = grd_accept,
};

/*********************************************************************
 Message Handler
*********************************************************************/
static void do_tcp_connect(struct y_message *msg)
{
	struct grd_netconn *netconn;
	struct sockaddr_in *in_addr;
	ip_addr_t local_ip;
	u16_t local_port;
	int ret;
	
	/* get netconn */
	y_message_read(msg, &netconn, &in_addr);

	/* set local port and ip */
	local_port    = in_addr->sin_port;
	local_ip.addr = in_addr->sin_addr.s_addr;

	/* Do connection by call the protocol stack */
	ret = tcp_connect(netconn->protocal_control_block, &local_ip, local_port, tcp_connected);
	ret = stack_err_to_posix_err(ret);
	y_message_writeback(msg, 1, ret);
	return;
}

static void do_bind(struct y_message *msg)
{
	struct grd_netconn *netconn;
	struct sockaddr_in *in_addr;
	u16_t local_port;
	ip_addr_t local_ip;
	struct tcp_pcb *pcb;
	
	err_t ret;
	
	y_message_read(msg, &netconn, &in_addr);
	pcb				= netconn->protocal_control_block;
	local_port		= in_addr->sin_port;
	local_ip.addr	= in_addr->sin_addr.s_addr;
	
	ret = tcp_bind(pcb, &local_ip, local_port);
	ret = stack_err_to_posix_err(ret);
	y_message_writeback(msg, 1, ret);
}

static void do_socket(struct y_message *msg)
{
	int ret = 0;
	unsigned int ops;
	int type;
	void *pcb = NULL;
	void *netcon_or_pcb;

	y_message_read(msg, &ops, &netcon_or_pcb);
	type = SOCKET_MANAGE_MSG_GET_ADD(ops);
	
	switch (SOCKET_MANAGE_MSG_GET_OPS(ops))
	{
		case SOCKET_MSG_ID_SUB_OPS_CREATE:
			if (type == SOCK_STREAM)
			{
				pcb = tcp_new();
				if (pcb == NULL)
					ret = -ENOMEM;
				else
					tcp_arg(pcb, netcon_or_pcb);
			}
			else
			{
				//TODO
				ret = ENOSYS;
			}
			break;
			
		case SOCKET_MSG_ID_SUB_OPS_DELETE:
			if (type == SOCK_STREAM)
			{
				tcp_abort(netcon_or_pcb);
			}
			else
			{
				//TODO
				ret = ENOSYS;
			}
		default:
			ret = -ENOSYS;
			break;
	}

	
	y_message_writeback(msg, 2, ret, pcb);
}

static void do_listen(struct y_message *msg)
{
	struct grd_netconn *netconn;
	int backlog;
	struct tcp_pcb *listen_pcb;
	int ret;
	
	y_message_read(msg, &netconn, &backlog);

	/* listen with backlog, orignal pcb is freed */
	if ((listen_pcb = tcp_listen_with_backlog(netconn->protocal_control_block, backlog)) == NULL)
		ret = -ENOMEM;
	else
	{
		netconn->protocal_control_block = listen_pcb;
		tcp_accept(listen_pcb, tcp_acceptted);
		ret = 0;
	}

	y_message_writeback(msg, 1, ret);
}

/*********************************************************************
 Protocal Stack Logic
*********************************************************************/

static bool setup_stream_file(struct net_interface *ctx)
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
	
	ring_buff_head_init(ctx->stream_map, 8192/*TODO: to get file size */);
	return true;

err1:
	y_file_close(ctx->stream_file);
err0:
	return false;
}

static void close_stream_file(struct net_interface *ctx)
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
	struct net_interface *ctx;
	
	y_message_read(msg, &ctx);

	/* 
		从消息中得到流文件，并读取流文件。
		此处可以用内存影射的方式替代read，从而
		减少一次内存拷贝。
	*/

	cache = (struct ring_buff_cache *)(ctx->stream_map);
	
	/* 获取可读报文 */
	while (NULL != (ring_pkt = ring_cache_read_package(cache)))
	{
		struct pbuf *pb;
		
		if (NULL == (pb = pbuf_alloc(PBUF_RAW, ring_pkt->package_size, PBUF_ZEROCOPY)))
			goto err;
		
		/* GCC BUG? */
		dummy_func(ring_pkt);
		
		pb->payload_org = pb->payload = (void *)cache + ring_pkt->package_offset;
		pb->zero_object = ring_pkt;
		ethernet_input(pb, &ctx->netif);
	}

	//TODO: 轮询流文件
	
	return;
	
err:
	printf("pbuf 分配失败\n");
	ctx->dropped++;
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
#ifdef USE_MAP_LOOP_BACK
		{
#define max_test (256 * 1024)/* no cache 直接系统调用，需要一整块 */
			static char test[max_test];
			struct ring_buff_cache *cache;
			void *raw_package;
			struct net_interface *nif = &global_net_interface;
			
			/* 拷贝数据，真是网卡则无需，而是使用0拷贝 */
			cache = (struct ring_buff_cache *)(nif->stream_map);
			raw_package = ring_buff_alloc(cache, q->len);
			memcpy(raw_package, q->payload, q->len);
			
			/* 触发事件 */
			y_file_read(nif->stream_file, &test, sizeof(test));
		}
#endif
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
	nif->name[0] = 'e';
	nif->name[1] = 't';
	netif_set_ipaddr(nif, &test_ipaddr);
	netif_set_netmask(nif, &test_netmask);
	netif_set_gw(nif, &test_gw);
		
	netif_set_up(nif);
	/* 这里用于测试tcp, 使用默认接口发送 */
	netif_set_default(nif);
}

static void *stream_input_worker(void *parameter)
{
	struct net_interface *pni = parameter;
	
	/* Startup netif */	
	nif_startup(&pni->netif);

	if (y_message_register(connect_msg_id, do_tcp_connect) != true)
		goto err0;
	if (y_message_register(bind_msg_id, do_bind) != true)
		goto err0;
	if (y_message_register(create_delete_socket_msg_id, do_socket) != true)
		goto err0;
	if (y_message_register(listen_msg_id, do_listen) != true)
		goto err0;
	
	
	/* Setup stream */
	if (false == setup_stream_file(pni))
		goto err0;
	if (y_file_event_register(pni->stream_file, Y_FILE_EVENT_READ, stream_input, pni) < 0)
		goto err1;

	/* Wait stream */
	y_message_loop();
err1:
	close_stream_file(pni);
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

void pbuf_free_zero_object(void* p)
{
	ring_buff_free_package(p);
}

unsigned int sys_now(void)
{
	printf("sys_now not implemented.\n");
	return 0;
}

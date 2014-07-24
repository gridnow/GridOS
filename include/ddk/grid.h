/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *
 *   Grid
 */
#ifndef DDK_GRID_H
#define DDK_GRID_H

#include <list.h>
#include <pthread.h>

#define NET_F_BLOCK  0x01

/**
	accept queue for tcp
*/
struct accept_queue
{
	/* list to struct netconn accept_queue */
	struct list_head list;
	
	/* 协议栈已经接受的tcp */
	void *accpeted_protocal_control_block;;
};

/**
	send packet message struct for socket layers
*/
struct send_msg_hd
{
	/* list to struct netconn recved or send list */
	struct list_head list;

	struct iovec
	{
		/* msg body */
		void *msg_base;
		/* msg total len and current write/read offset */
		unsigned long msg_len;
		unsigned long offset;

		/* for msg dest name, always struct socket_inaddr */
		void *msg_name;
		int msg_name_len;
		
	}*iovec;
	
	int iovec_offset;
	int iovec_count;
};

/* 接收mesg struct */
struct recv_msg_hd
{
	struct list_head list;
	
	void *stack_msg;
	/* 当前消息读取偏移量 */
	int offset;
};

/* 记录socket与协议栈相关信息 */
struct grd_netconn
{
	/* 网络类型标志 tcp udp raw_ip */
	u16 net_types;
	/* 阻塞与非阻塞标志 */
	u16 flag;
	
	/* 用于链接和数据的唤醒 */
	y_handle event;
	
	/* netconn同步 */
	pthread_spinlock_t netconn_lock;
	
	/* 协议栈控制块 */
	void *protocal_control_block;
  	
  	/* struct accep_queue head */
  	struct list_head accep_queue;

	/* send and recved packet msg list head */
	struct list_head recv_queue;
	struct list_head send_queue;
	/* 用于协议栈线程释放pbuf */
	struct list_head wait_free_queue;
	/* 最多可以缓存多少个报文 */
	int rx_queue_limit;
	/* 协议栈线程操作后的返回结果,用于sock等待函数 */
	int result;
};

/**
	@brief 用户使用该接口进行数据的收发
*/
struct grid_netproto
{
	int version;
	const char *proto_name;
	void *(*socket)(int type, int proto);
	/* Protocol ops */
	int (*connect)(struct grd_netconn *netconn, const void *addr, size_t addr_len);
	int (*bind)(struct grd_netconn *netconn, void *addr, size_t addr_len);
	int (*listen)(struct grd_netconn *netconn, int backlog);
	void *(*accept)(struct grd_netconn *netconn, void *addr, size_t *addr_len);
	int (*send)(struct grd_netconn *netconn, void *buff, size_t len, int flag);
	int (*recv)(struct grd_netconn *netconn, void *buff, size_t len, int flag);
};

#define GRID_GET_NETPROTO "grid_acquire_netproto"

#endif


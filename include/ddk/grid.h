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
	
	/* Э��ջ�Ѿ����ܵ�tcp */
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

/* ����mesg struct */
struct recv_msg_hd
{
	struct list_head list;
	
	void *stack_msg;
	/* ��ǰ��Ϣ��ȡƫ���� */
	int offset;
};

/* ��¼socket��Э��ջ�����Ϣ */
struct grd_netconn
{
	/* �������ͱ�־ tcp udp raw_ip */
	u16 net_types;
	/* �������������־ */
	u16 flag;
	
	/* �������Ӻ����ݵĻ��� */
	y_handle event;
	
	/* netconnͬ�� */
	pthread_spinlock_t netconn_lock;
	
	/* Э��ջ���ƿ� */
	void *protocal_control_block;
  	
  	/* struct accep_queue head */
  	struct list_head accep_queue;

	/* send and recved packet msg list head */
	struct list_head recv_queue;
	struct list_head send_queue;
	/* ����Э��ջ�߳��ͷ�pbuf */
	struct list_head wait_free_queue;
	/* �����Ի�����ٸ����� */
	int rx_queue_limit;
	/* Э��ջ�̲߳�����ķ��ؽ��,����sock�ȴ����� */
	int result;
};

/**
	@brief �û�ʹ�øýӿڽ������ݵ��շ�
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


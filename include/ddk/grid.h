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
};

#define GRID_GET_NETPROTO "grid_acquire_netproto"

#endif


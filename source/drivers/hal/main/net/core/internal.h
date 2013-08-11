#ifndef NET_CORE_INTERNAL_H
#define NET_CORE_INTERNAL_H

#include "blkbuf.h"

typedef struct
{
	struct ddk_net_device dev;
	struct cl_bkb nb_bkb, d_bkb;
} net_device_t;

/**
	@brief 初始化设备的rt功能

	一般在创立网络设备对象时调用
 
	@param [in] dev 网络设备对象，调用者保证为正确对象
 
	@return
		0 为成功, < 0 表示错误
*/
int netdev_rt_init(net_device_t *dev);

/**
	@brief 创立网络设备对象
*/
net_device_t *alloc_netdev_mqs(int sizeof_priv, const char *name,
							   void (*setup)(net_device_t *),
							   unsigned int txqs, unsigned int rxqs);
#endif
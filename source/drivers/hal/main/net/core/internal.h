#ifndef NET_CORE_INTERNAL_H
#define NET_CORE_INTERNAL_H

#include "blkbuf.h"

typedef struct
{
	struct ddk_net_device dev;
	struct cl_bkb nb_bkb, d_bkb;
} net_device_t;

/**
	@brief ��ʼ���豸��rt����

	һ���ڴ��������豸����ʱ����
 
	@param [in] dev �����豸���󣬵����߱�֤Ϊ��ȷ����
 
	@return
		0 Ϊ�ɹ�, < 0 ��ʾ����
*/
int netdev_rt_init(net_device_t *dev);

/**
	@brief ���������豸����
*/
net_device_t *alloc_netdev_mqs(int sizeof_priv, const char *name,
							   void (*setup)(net_device_t *),
							   unsigned int txqs, unsigned int rxqs);
#endif
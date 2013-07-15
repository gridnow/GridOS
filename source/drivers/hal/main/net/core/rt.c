/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   Receiving and Transmiting of net devices
 */
#include <ddk/types.h>
#include <ddk/debug.h>
#include <ddk/string.h>
#include <ddk/net/rt.h>
#include <ddk/net/netdevice.h>
#include <ddk/slab.h>

#include "internal.h"

#define NB_BKB_EXT_SIZE 1024
#define NB_D_EXT_SIZE 8192 /* Cannot less then MTU */

static void free_nb_bkb(void *para, void *base, size_t size)
{
	hal_free(base);
}

static void free_d_bkb(void *para, void *base, size_t size)
{
	hal_free(base);
}

static struct ddk_net_buf *alloc_nb(net_device_t *dev)
{
	void *ext = NULL;
	struct ddk_net_buf *p;
	
again:
	p = cl_bkb_alloc(&dev->nb_bkb);
	if (p) goto end;
	
	if (ext) goto end;
	ext = hal_malloc(NB_BKB_EXT_SIZE);
	if (!ext) goto end;
	cl_bkb_extend(&dev->nb_bkb, ext, NB_BKB_EXT_SIZE, free_nb_bkb, NULL);
	goto again;
	
end:
	if (p)
	{
		memset(p, 0, sizeof(*p));
		p->dev = (struct ddk_net_device*)dev;
		INIT_LIST_HEAD(&p->list);
	}
	return p;
}

static void dealloc_nb(net_device_t *dev, struct ddk_net_buf *buf)
{
	//TODO
}

static int append_data(struct ddk_net_buf *buf, void *data, size_t size)
{
	size_t i, old, cur_pos = 0;
	net_device_t *dev;
	struct ddk_net_buf *old_nb;
	
	dev			= (net_device_t*)buf->dev;
	old			= buf->last_buf;
	old_nb		= buf;

	while (1)
	{
		if (buf->last_buf < DDK_NET_BUF_MAX_SLICE)
		{
			void *d = cl_bkb_alloc(&dev->d_bkb);
			if (!d)
			{
				void * ext = hal_malloc(NB_D_EXT_SIZE);
				if (!ext) goto err1;
				cl_bkb_extend(&dev->d_bkb, ext, NB_D_EXT_SIZE, free_d_bkb, NULL);
				d = cl_bkb_alloc(&dev->d_bkb);
			}
			
			buf->buf[buf->last_buf++] = d;
			if (cur_pos + dev->d_bkb.node_size > size)
			{
				memcpy(d, data + cur_pos, size - cur_pos);
				buf->last_size = size - cur_pos;
				break;
			}
			else
				memcpy(d, data + cur_pos, dev->d_bkb.node_size);
			cur_pos += dev->d_bkb.node_size;
		}
		else
		{
			struct ddk_net_buf *new_nb;
			
			new_nb = alloc_nb(dev);
			if (!new_nb)
				goto err2;
			
			list_add_tail(&new_nb->list, &buf->list);
			buf = new_nb;
		}
	}
	
	return 0;
	
err2:
err1:
	//TODO recall all NB
	;
}

/***********************************************************************
	Internal interfaces
***********************************************************************/
int netdev_rt_init(net_device_t *dev)
{
	cl_bkb_init(&dev->nb_bkb, "net_rt_bkb", sizeof(struct ddk_net_buf));
	cl_bkb_init(&dev->d_bkb, "net_data_bkb", dev->dev.mtu);
	return 0;
}

/***********************************************************************
	Export interfaces
***********************************************************************/
struct ddk_net_buf *netdev_nb_alloc(struct ddk_net_device *dev)
{
	return alloc_nb((net_device_t*)dev);
}

void netdev_nb_dealloc(struct ddk_net_buf *buf)
{
	dealloc_nb((net_device_t*)buf->dev, buf);
}

int netdev_nb_append_data(struct ddk_net_buf *buf, void *data, size_t size)
{
	return append_data(buf, data, size);
}

netdev_tx_t netdev_start_transmit(struct ddk_net_device *dev, struct ddk_net_buf *buf)
{
	return dev->netdev_ops->start_xmit(buf, dev);
}


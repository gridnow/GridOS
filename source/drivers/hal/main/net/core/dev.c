/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   Net devices manager
 */

#include <ddk/types.h>
#include <ddk/debug.h>
#include <ddk/compiler.h>
#include <ddk/string.h>
#include <ddk/net/netdevice.h>
#include <ddk/slab.h>

#include <kernel/ke_lock.h>
#include <stddef.h>

#include "internal.h"

static struct ke_spinlock netdev_list_lock;
static LIST_HEAD(netdev_list);

net_device_t *alloc_netdev_mqs(int sizeof_priv, const char *name,
									void (*setup)(net_device_t *),
									unsigned int txqs, unsigned int rxqs)
{
	net_device_t *dev;
	size_t alloc_size;
	net_device_t *p;
	
	alloc_size = sizeof(net_device_t); 
	if (sizeof_priv) {
		/* ensure 32-byte alignment of private area */
		alloc_size = ALIGN(alloc_size, NETDEV_ALIGN);
		alloc_size += sizeof_priv;
	}
	/* ensure 32-byte alignment of whole construct */
	alloc_size += NETDEV_ALIGN - 1;
	
	p = kzalloc(alloc_size, GFP_KERNEL);
	if (!p) {
		return NULL;
	}
	
	dev = PTR_ALIGN(p, NETDEV_ALIGN);
	setup(dev);
	
	/* ÊÕ·¢»º³åÇø */
	netdev_rt_init(dev);
	return dev;
	
free_p:
	kfree(p);
	return NULL;
}

int netdev_register(struct ddk_net_device *dev)
{
	ke_spin_lock(&netdev_list_lock);
	list_add_tail(&dev->dev_list, &netdev_list);
	ke_spin_unlock(&netdev_list_lock);
	
	return 0;
}

int netdev_open(struct ddk_net_device *dev)
{
	int ret = 0;
	const struct ddk_net_device_ops *ops = dev->netdev_ops;
	
	if (!ret && ops->open)
		ret = ops->open(dev);
}

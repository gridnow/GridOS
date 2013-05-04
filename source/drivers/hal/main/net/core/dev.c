#include <ddk/types.h>
#include <ddk/debug.h>
#include <ddk/compiler.h>
#include <ddk/string.h>
#include <ddk/net/netdevice.h>
#include <ddk/slab.h>

#include <stddef.h>


struct net_device *alloc_netdev_mqs(int sizeof_priv, const char *name,
									void (*setup)(struct net_device *),
									unsigned int txqs, unsigned int rxqs)
{
	struct net_device *dev;
	size_t alloc_size;
	struct net_device *p;
	
	alloc_size = sizeof(struct net_device);
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
	return dev;
	
free_p:
	kfree(p);
	return NULL;
}

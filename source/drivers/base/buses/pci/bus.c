#include <ddk/types.h>
#include <ddk/log.h>
#include <ddk/io.h>
#include <ddk/irq.h>
#include <ddk/resource.h>
#include <ddk/slab.h>
#include <ddk/pci/pci_regs.h>
#include <ddk/pci/class.h>

#include <compiler.h>
#include <list.h>
#include <errno.h>
#include <string.h>

#include "pci.h"

void pci_add_resource_offset(struct list_head *resources, struct resource *res,
	resource_size_t offset)
{
	struct pci_host_bridge_window *window;

	window = kzalloc(sizeof(struct pci_host_bridge_window), GFP_KERNEL);
	if (!window) {
		printk(KERN_ERR "PCI: can't add host bridge window %pR\n", res);
		return;
	}

	window->res = res;
	window->offset = offset;
	list_add_tail(&window->list, resources);
}

void pci_bus_add_resource(struct pci_bus *bus, struct resource *res,
						  unsigned int flags)
{
	struct pci_bus_resource *bus_res;

	bus_res = kzalloc(sizeof(struct pci_bus_resource), GFP_KERNEL);
	if (!bus_res) {
		dev_dbg(&bus->dev, "没有空闲内存来分配pci_bus_resource.\n");
		return;
	}

	bus_res->res = res;
	bus_res->flags = flags;
	list_add_tail(&bus_res->list, &bus->resources);
}

void pci_add_resource(struct list_head *resources, struct resource *res)
{
	pci_add_resource_offset(resources, res, 0);
}

void pci_free_resource_list(struct list_head *resources)
{
	struct pci_host_bridge_window *window, *tmp;

	list_for_each_entry_safe(window, tmp, resources, list) {
		list_del(&window->list);
		kfree(window);
	}
}

struct resource *pci_bus_resource_n(const struct pci_bus *bus, int n)
{
	struct pci_bus_resource *bus_res;

	if (n < PCI_BRIDGE_RESOURCE_NUM)
		return bus->resource[n];

	n -= PCI_BRIDGE_RESOURCE_NUM;
	list_for_each_entry(bus_res, &bus->resources, list) {
		if (n-- == 0)
			return bus_res->res;
	}
	return NULL;
}

void pci_bus_remove_resources(struct pci_bus *bus)
{
	int i;

	for (i = 0; i < PCI_BRIDGE_RESOURCE_NUM; i++)
		bus->resource[i] = NULL;

	pci_free_resource_list(&bus->resources);
}

int pci_bus_add_device(struct pci_dev *dev)
{
	int retval;

	pci_fixup_device(pci_fixup_final, dev);

	/* 注册，并设置名字 */
	do_register_device(dev, dev->bus/*该设备的BUS应该是其父设备，但是在pci_setup_device中，parent是bridge*/);
	do_set_device_name(dev, "%04x:%02x:%02x.%d", pci_domain_nr(dev->bus),
		dev->bus->number, PCI_SLOT(dev->devfn),
		PCI_FUNC(dev->devfn));

	dev->is_added = 1;
//	pci_proc_attach_device(dev); 
//	pci_create_sysfs_dev_files(dev);

	return 0;
}

int pci_bus_add_child(struct pci_bus *bus)
{
#if 0
	int retval;

	if (bus->bridge)
		bus->dev.parent = bus->bridge;

	retval = device_register(&bus->dev);
	if (retval)
		return retval;

	bus->is_added = 1;

	/* Create legacy_io and legacy_mem files for this bus */
	pci_create_legacy_files(bus);

	return retval;
#endif
	//do_set_device_name(bus, "%04x:%02x", pci_domain_nr(bus), busnr);

	printk("%s not impelemented.\n", __FUNCTION__);
	return 0;
}

void pci_bus_add_devices(const struct pci_bus *bus)
{
	struct pci_dev *dev;
	struct pci_bus *child;
	int retval;

	list_for_each_entry(dev, &bus->devices, bus_list) {
		/* Skip already-added devices */
		if (dev->is_added)
			continue;
		retval = pci_bus_add_device(dev);
		if (retval)
			dev_err(&dev->dev, "Error adding device, continuing\n");
	}

	list_for_each_entry(dev, &bus->devices, bus_list) {
		//BUG_ON(!dev->is_added);
		if (!dev->is_added)
		{
			dev_err(dev, "%s %d 错误.\n", __FUNCTION__, __LINE__);
			return;
		}

		child = dev->subordinate;
		/*
		 * If there is an unattached subordinate bus, attach
		 * it and then scan for unattached PCI devices.
		 */
		if (!child)
			continue;
		if (list_empty(&child->node)) {
			pci_bus_lock();
			list_add_tail(&child->node, &dev->bus->children);
			pci_bus_unlock();
		}
		pci_bus_add_devices(child);

		/*
		 * register the bus with sysfs as the parent is now
		 * properly registered.
		 */
		if (child->is_added)
			continue;
		retval = pci_bus_add_child(child);
		if (retval)
			dev_err(&dev->dev, "Error adding bus, continuing\n");
	}
}

/** pci_walk_bus - walk devices on/under bus, calling callback.
 *  @top      bus whose devices should be walked
 *  @cb       callback to be called for each device found
 *  @userdata arbitrary pointer to be passed to callback.
 *
 *  Walk the given bus, including any bridged devices
 *  on buses under this bus.  Call the provided callback
 *  on each device found.
 *
 *  We check the return of @cb each time. If it returns anything
 *  other than 0, we break out.
 *
 */
void pci_walk_bus(struct pci_bus *top, int (*cb)(struct pci_dev *, void *),
		  void *userdata)
{
	struct pci_dev *dev;
	struct pci_bus *bus;
	struct list_head *next;
	int retval;

	bus = top;
	pci_bus_lock();
	next = top->devices.next;
	for (;;) {
		if (next == &bus->devices) {
			/* end of this bus, go up or finish */
			if (bus == top)
				break;
			next = bus->self->bus_list.next;
			bus = bus->self->bus;
			continue;
		}
		dev = list_entry(next, struct pci_dev, bus_list);
		if (dev->subordinate) {
			/* this is a pci-pci bridge, do its devices next */
			next = dev->subordinate->devices.next;
			bus = dev->subordinate;
		} else
			next = dev->bus_list.next;

		/* Run device routines with the device locked */
		//device_lock(&dev->dev);
		retval = cb(dev, userdata);
		//device_unlock(&dev->dev);
		if (retval)
			break;
	}
	pci_bus_unlock();
}
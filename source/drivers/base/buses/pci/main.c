/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   PCI Manager
 */
#include <ddk/types.h>
#include <ddk/string.h>
#include <ddk/debug.h>
#include <ddk/compatible.h>
#include <ddk/dma.h>

#include "pci.h"

#include <ddk/pci/pci.h>


static bool match_driver(void * dev, void * driver)
{
	struct pci_dev * pdev = dev;
	struct ddk_pci_driver * pdrv = driver;
	int result = -1;

	/* 调用驱动的probe来探测 */
	if (pdrv->probe)
	{
		const struct pci_device_id * id;
		/* ID MET? */
		if ((id = pci_match_id(pdrv->id_table, pdev)) != NULL)
		{
// 			printk("PCI总线驱动正在为设备%x匹配驱动%s...", pdev, pdrv->name);
			result = pdrv->probe(pdev, id);
//			printk("匹配%s.\n", result == 0 ? "成功":"失败");
		}
	}
	
	return result == 0 ? true:false;
}

struct do_device_type pci_dev_type =
{
	.name 				= "PCI类型的设备",
	.size_of_device 	= sizeof(struct pci_dev),
	.match 				= match_driver,
};

LIST_HEAD(quirk_final);
static DEFINE_SPINLOCK(quirk_lock);

DLLEXPORT struct do_device_type * pci_get_device_type()
{
	return &pci_dev_type;
}

DLLEXPORT bool pci_register_driver(struct ddk_pci_driver * drv)
{
	return do_register_driver(drv, pci_get_device_type());
}

DLLEXPORT void pci_unregister_driver(struct ddk_pci_driver * drv)
{
	TODO("");
}

DLLEXPORT struct resource * pci_resource(struct pci_dev * pdev)
{
	return pdev->resource;
}

DLLEXPORT void __iomem *pci_ioremap_bar(struct pci_dev *pdev, int bar)
{
	TODO("");
}

DLLEXPORT void pci_register_final_quirk(struct pci_fixup * q)
{
	spin_lock(&quirk_lock);
	list_add_tail(&q->list, &quirk_final);
	spin_unlock(&quirk_lock);
}

DLLEXPORT int pci_set_dma_mask(struct pci_dev *dev, u64 mask)
{
	return ddk_dma_set_mask(&dev->dev, mask);	
}

int pci_set_consistent_dma_mask(struct pci_dev *dev, u64 mask)
{
	TODO("");
	return 0;
}

DLLEXPORT void pcim_pin_device(struct pci_dev *pdev)
{
	UNUSED("");
}


DLLEXPORT void __iomem * const *pcim_iomap_table(struct pci_dev *pdev)
{
	TODO("");
	return NULL;
}

/************************************************************************/
/* Interface for DDK of driver subsystem                                */
/************************************************************************/
#include <ddk/ddk_for_linux.h>
void pci_link_irq_number(void *(*link_method)(void *struct_pci_device_id, void *hal_device, int irq))
{
	void *pdev;
	struct pci_dev *dev = NULL;
	struct pci_device_id id[2] = {0};

	for_each_pci_dev(dev) {
		memset(&id, 0, sizeof(id));
		id[0].class = dev->class;
		id[0].device = dev->device;
		id[0].subdevice = dev->subsystem_device;
		id[0].subvendor = dev->subsystem_vendor;
		id[0].vendor = dev->vendor;

		/* Accurite additional match */
		id[0].driver_data = dev->devfn;

		pdev = link_method(id, dev/*Make driver-subsystem link back*/, dev->irq);
		/* Now we do not need to link back to pdev */
	}
}

int pci_root_operation(unsigned int domain, unsigned int bus_nr, unsigned int devfn, int where, int size, void *u32_val, int write)
{
	u32 *val = u32_val;

	struct pci_raw_ops {
		int (*read)(unsigned int domain, unsigned int bus, unsigned int devfn,
			int reg, int len, u32 *val);
		int (*write)(unsigned int domain, unsigned int bus, unsigned int devfn,
			int reg, int len, u32 val);
	};
	/* Every arch should has this */
	extern struct pci_raw_ops * raw_pci_ops;
	if (write == false)
		return raw_pci_ops->read(domain, bus_nr, devfn, where, size, val);
	else
		return raw_pci_ops->write(domain, bus_nr, devfn, where, size, (u32)val);
}

int  pci_bios_enable_device(void *pdev, int mask)
{
	struct pci_dev *dev = pdev;
	return pcibios_enable_device(dev, mask);
}

void pci_bios_disable_device(void *pdev)
{
	struct pci_dev *dev = pdev;
	pcibios_disable_device(dev);
}

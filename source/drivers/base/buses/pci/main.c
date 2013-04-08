
#include <ddk/string.h>
#include <ddk/log.h>

#include "pci.h"

#include <ddk/pci/pci.h>
#include <kernel/ke_lock.h>

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
	dev->dma_mask = mask;

	return 0;
}

DLLEXPORT int pci_set_consistent_dma_mask(struct pci_dev *dev, u64 mask)
{
	TODO("");
	return 0;
}

DLLEXPORT void pcim_pin_device(struct pci_dev *pdev)
{
	UNUSED("");
}

DLLEXPORT int pci_enable_msi_block(struct pci_dev *dev, unsigned int nvec)
{
	UNUSED("没有配置该功能");
	return -ENOSYS;
}

DLLEXPORT void __iomem * const *pcim_iomap_table(struct pci_dev *pdev)
{
	TODO("");
	return NULL;
}
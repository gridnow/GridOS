/**
	8138 C+ Íø¿¨Çý¶¯
*/
#include <errno.h>

#include <ddk/log.h>
#include <ddk/pci/pci.h>
#include <ddk/pci/global_ids.h>

#define DRV_NAME		"8139cp"

static DEFINE_PCI_DEVICE_TABLE(cp_pci_tbl) = {
	{ PCI_DEVICE(PCI_VENDOR_ID_REALTEK,	PCI_DEVICE_ID_REALTEK_8139), },
	{ PCI_DEVICE(PCI_VENDOR_ID_TTTECH,	PCI_DEVICE_ID_TTTECH_MC322), },
	{ },
};

static int cp_init_one (struct pci_dev *pdev, const struct pci_device_id *ent)
{
	int rc;

	/* May be old 8139 */
	if (pdev->vendor == PCI_VENDOR_ID_REALTEK &&
		pdev->device == PCI_DEVICE_ID_REALTEK_8139 && pdev->revision < 0x20) {
			dev_info(&pdev->dev,
				"This (id %04x:%04x rev %02x) is not an 8139C+ compatible chip, use 8139too\n",
				pdev->vendor, pdev->device, pdev->revision);
			return -ENODEV;
	}

	dev_info(&pdev->dev,
		"This (id %04x:%04x rev %02x) is an 8139C+ compatible chip.\n",
		pdev->vendor, pdev->device, pdev->revision);

	/* Enable the pci device */
	rc = pci_enable_device(pdev);
	if (rc)
		goto err;

	return 0;

err:
	return rc;
}

static void cp_remove_one (struct pci_dev *pdev)
{

}

struct ddk_pci_driver cp_driver = {
	.name			= DRV_NAME,
	.id_table		= cp_pci_tbl,
	.probe			= cp_init_one,
	.remove			= cp_remove_one,
};

static int __init cp_init (void)
{
	return pci_register_driver(&cp_driver);
}

static void cp_exit (void)
{
	pci_unregister_driver (&cp_driver);
}

driver_initcall(cp_init);
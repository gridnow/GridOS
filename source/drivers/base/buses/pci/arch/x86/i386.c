#include <ddk/types.h>
#include <ddk/log.h>
#include <ddk/io.h>
#include <ddk/irq.h>
#include <ddk/resource.h>
#include <ddk/topology.h>
#include <ddk/pci/pci_regs.h>
#include <ddk/pci/class.h>
#include <ddk/pci/global_ids.h>

#include <FIRMWARE/dmi/dmi.h> 

#include <list.h>
#include <errno.h>
#include <string.h>

#include <pci.h>

#include "pci.h"
#include "pci_x86.h"

/*
 *  If we set up a device for bus mastering, we need to check the latency
 *  timer as certain crappy BIOSes forget to set it properly.
 */
unsigned int pcibios_max_latency = 255;

void pcibios_set_master(struct pci_dev *dev)
{
	u8 lat;
	pci_read_config_byte(dev, PCI_LATENCY_TIMER, &lat);
	if (lat < 16)
		lat = (64 <= pcibios_max_latency) ? 64 : pcibios_max_latency;
	else if (lat > pcibios_max_latency)
		lat = pcibios_max_latency;
	else
		return;
	dev_printk(KERN_DEBUG, &dev->dev, "setting latency timer to %d\n", lat);
	pci_write_config_byte(dev, PCI_LATENCY_TIMER, lat);
}
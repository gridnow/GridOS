
#include <ddk/log.h>
#include <ddk/obj.h>

#include "pci.h"

const struct pci_device_id *pci_match_id(const struct pci_device_id *ids,
					 struct pci_dev *dev)
{
	if (ids) {
		while (ids->vendor || ids->subvendor || ids->class_mask) {
			if (pci_match_one_device(ids, dev))
				return ids;
			ids++;
		}
	}
	return NULL;
}

struct pci_dev *pci_dev_get(struct pci_dev *dev)
{
	if (dev)
		do_get_device(dev);
	
	return dev;
}

DLLEXPORT void pci_dev_put(struct pci_dev *dev)
{
	if (dev)
		do_put_device(dev);	
}


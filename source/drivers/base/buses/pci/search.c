
#include <types.h>
#include <ddk/log.h>
#include <kernel/ke_lock.h>

#include "pci.h"

static DEFINE_SPINLOCK(pci_bus_spinlock);

static bool match_pci_dev_by_id(void *dev, void *data)
{
	struct pci_dev *pdev = dev;
	struct pci_device_id *id = data;

	if (pci_match_one_device(id, pdev))
		return true;
	return false;
}

/*
 * pci_get_dev_by_id - begin or continue searching for a PCI device by id
 */
static struct pci_dev *pci_get_dev_by_id(const struct pci_device_id *id,
					 struct pci_dev *from)
{
	struct pci_dev *dev;
	struct pci_dev *dev_start = NULL;
	struct pci_dev *pdev = NULL;

	if (from)
		dev_start = from;

	dev = do_find_device(pci_get_device_type(), dev_start, (void *)id,
			      match_pci_dev_by_id);

	if (dev)
		pdev = dev;
	if (from)
		pci_dev_put(from);
	return pdev;
}

static struct pci_bus *pci_do_find_bus(struct pci_bus *bus, unsigned char busnr)
{
	struct pci_bus* child;
	struct list_head *tmp;

	if(bus->number == busnr)
		return bus;

	list_for_each(tmp, &bus->children) {
		child = pci_do_find_bus(pci_bus_b(tmp), busnr);
		if(child)
			return child;
	}
	return NULL;
}

struct pci_bus * pci_find_bus(int domain, int busnr)
{
	struct pci_bus *bus = NULL;
	struct pci_bus *tmp_bus;

	while ((bus = pci_find_next_bus(bus)) != NULL)  {
		if (pci_domain_nr(bus) != domain)
			continue;
		tmp_bus = pci_do_find_bus(bus, busnr);
		if (tmp_bus)
			return tmp_bus;
	}
	return NULL;
}

/**
 * pci_find_next_bus - begin or continue searching for a PCI bus
 * @from: Previous PCI bus found, or %NULL for new search.
 *
 * Iterates through the list of known PCI busses.  A new search is
 * initiated by passing %NULL as the @from argument.  Otherwise if
 * @from is not %NULL, searches continue from next device on the
 * global list.
 */
struct pci_bus *  pci_find_next_bus(const struct pci_bus *from)
{
	struct list_head *n;
	struct pci_bus *b = NULL;

	pci_bus_lock();
	n = from ? from->node.next : pci_root_buses.next;
	if (n != &pci_root_buses)
		b = pci_bus_b(n);
	pci_bus_unlock();

	return b;
}

struct pci_dev * pci_get_slot(struct pci_bus *bus, unsigned int devfn)
{
	struct list_head *tmp;
	struct pci_dev *dev;

	pci_bus_lock();

	list_for_each(tmp, &bus->devices) {
		dev = pci_dev_b(tmp);
		if (dev->devfn == devfn)
			goto out;
	}

	dev = NULL;
out:
	pci_dev_get(dev);
	pci_bus_unlock();
	return dev;
}

struct pci_dev *pci_get_domain_bus_and_slot(int domain, unsigned int bus, unsigned int devfn)
{
	struct pci_dev *dev = NULL;

	for_each_pci_dev(dev) {
		if (pci_domain_nr(dev->bus) == domain &&
			(dev->bus->number == bus && dev->devfn == devfn))
			return dev;
	}

	return NULL;
}

struct pci_dev *pci_get_subsys(unsigned int vendor, unsigned int device,
			       unsigned int ss_vendor, unsigned int ss_device,
			       struct pci_dev *from)
{
	struct pci_dev *pdev;
	struct pci_device_id t = {0}, *id = &t;

	id->vendor = vendor;
	id->device = device;
	id->subvendor = ss_vendor;
	id->subdevice = ss_device;

	pdev = pci_get_dev_by_id(id, from);

	return pdev;
} 

DLLEXPORT struct pci_dev * pci_get_device(unsigned int vendor, unsigned int device, struct pci_dev *from)
{
	return pci_get_subsys(vendor, device, PCI_ANY_ID, PCI_ANY_ID, from);
}

int pci_dev_present(const struct pci_device_id *ids)
{
	struct pci_dev *found = NULL;

	WARN_ON(in_interrupt());
	while (ids->vendor || ids->subvendor || ids->class_mask) {
		found = pci_get_dev_by_id(ids, NULL);
		if (found)
			goto exit;
		ids++;
	}
exit:
	if (found)
		return 1;
	return 0;
}

void pci_bus_lock()
{
	spin_lock(&pci_bus_spinlock);
}

void pci_bus_unlock()
{
	spin_unlock(&pci_bus_spinlock);
}
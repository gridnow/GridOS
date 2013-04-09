#include "pci.h"

#include <ddk/pci/pci.h>
#include <ddk/log.h>
static void pci_do_fixups(struct pci_dev *dev, struct list_head * t)
{
	struct pci_fixup * f;
	
	list_for_each_entry(f, t, list)
		if ((f->class == (u32) (dev->class >> f->class_shift) ||
		     f->class == (u32) PCI_ANY_ID) &&
		    (f->vendor == dev->vendor ||
		     f->vendor == (u16) PCI_ANY_ID) &&
		    (f->device == dev->device ||
		     f->device == (u16) PCI_ANY_ID)) {
			f->hook(dev);
		}
}

void pci_fixup_device(enum pci_fixup_pass pass, struct pci_dev *dev)
{
	struct list_head *t = NULL;

	switch(pass) {
	case pci_fixup_early:
		break;

	case pci_fixup_header:
		break;

	case pci_fixup_final:
		t = &quirk_final;
		break;

	case pci_fixup_enable:
		break;

	case pci_fixup_resume:
		break;

	case pci_fixup_resume_early:
		break;

	case pci_fixup_suspend:
		break;

	default:
		/* stupid compiler warning, you would think with an enum... */
		return;
	}

	if (!t) return;
	pci_do_fixups(dev, t);
}
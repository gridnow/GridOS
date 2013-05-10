#include "pci.h"

#include <ddk/pci/pci.h>
#include <ddk/pci/pci_regs.h>
#include <ddk/log.h>

static bool pci_apply_fixup_final_quirks;

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

int __init pci_apply_final_quirks(void)
{
	struct pci_dev *dev = NULL;
	u8 cls = 0;
	u8 tmp;
	
	if (pci_cache_line_size)
		printk(KERN_DEBUG "PCI: CLS %u bytes\n",
		       pci_cache_line_size << 2);
	
	pci_apply_fixup_final_quirks = true;
	for_each_pci_dev(dev) {
		pci_fixup_device(pci_fixup_final, dev);
		/*
		 * If arch hasn't set it explicitly yet, use the CLS
		 * value shared by all PCI devices.  If there's a
		 * mismatch, fall back to the default value.
		 */
		if (!pci_cache_line_size) {
			pci_read_config_byte(dev, PCI_CACHE_LINE_SIZE, &tmp);
			if (!cls)
				cls = tmp;
			if (!tmp || cls == tmp)
				continue;
			
			printk(KERN_DEBUG "PCI: CLS mismatch (%u != %u), "
			       "using %u bytes\n", cls << 2, tmp << 2,
			       pci_dfl_cache_line_size << 2);
			pci_cache_line_size = pci_dfl_cache_line_size;
		}
	}
	
	if (!pci_cache_line_size) {
		printk(KERN_DEBUG "PCI: CLS %u bytes, default %u\n",
		       cls << 2, pci_dfl_cache_line_size << 2);
		pci_cache_line_size = cls ? cls : pci_dfl_cache_line_size;
	}
	
	return 0;
}
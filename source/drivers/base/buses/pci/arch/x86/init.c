#include <ddk/types.h>
#include <ddk/log.h>

#include <compiler.h>
#include <list.h>

#include <pci.h>
#include "pci_x86.h"
struct x86_init_ops x86_init_pci = {
	.pci = {
		.init			= x86_default_pci_init,
		.init_irq		= x86_default_pci_init_irq,
		.fixup_irqs		= x86_default_pci_fixup_irqs,
	},
};

/* arch_initcall has too random ordering, so call the initializers
   in the right sequence from here. */
__init int pci_arch_init(void)
{
#ifdef CONFIG_PCI_DIRECT
	int type = 0;
	type = pci_direct_probe();
#endif

 	if (!(pci_probe & PCI_PROBE_NOEARLY))
 		pci_mmcfg_early_init();
 
 	if (x86_init_pci.pci.arch_init && !x86_init_pci.pci.arch_init())
 		return 0;

#ifdef CONFIG_PCI_BIOS
	pci_pcbios_init();
#endif
	/*
	 * don't check for raw_pci_ops here because we want pcbios as last
	 * fallback, yet it's needed to run first to set pcibios_last_bus
	 * in case legacy PCI probing is used. otherwise detecting peer busses
	 * fails.
	 */
#ifdef CONFIG_PCI_DIRECT
	pci_direct_init(type);
#endif
	if (!raw_pci_ops && !raw_pci_ext_ops)
		printk(KERN_ERR
		"PCI: Fatal: No config space access function found\n");

	dmi_check_pciprobe();

	dmi_check_skip_isa_align();

	return 0;
}


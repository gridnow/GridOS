
#include <compiler.h>
#include <types.h>

DLLEXPORT bool ke_module_entry(int type)
{
	/* PCI init */
#ifdef __i386__ 
//	usb_register_quirk();

	pci_arch_init();
	pci_subsys_init();
	pci_apply_final_quirks();/* Setup the pci_cache_line_size */
#endif

}

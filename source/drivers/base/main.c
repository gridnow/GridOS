
#include <compiler.h>
#include <types.h>
#include <string.h>


DLLEXPORT bool ke_module_entry(int type)
{
	//start_kbd();

	/* PCI init */
#ifdef __i386__ 
//	usb_register_quirk();

	pci_arch_init();
	pci_subsys_init();
	pci_apply_final_quirks();/* Setup the pci_cache_line_size */
#endif

	/* ������Ƕ����ģ�� */
	printk("Starting embedded drivers...\n");
	start_embeded_driver();	
}

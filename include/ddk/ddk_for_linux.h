#ifndef DDK_FOR_LINUX_H
#define DDK_FOR_LINUX_H

/* Types that have to be used by the interface */
struct fss_vfs_driver;
struct nss_hwmgr;
struct ddk_input_handle;

struct ddk_for_linux
{
	int (*printk)(const char *fmt, ...);
	void (*allocate_physical_bulk)(unsigned long *start, unsigned long *size, unsigned long map_base);

	/************************************************************************/
	/* IRQ                                                                  */
	/************************************************************************/
	void (*setup_irq_handler)(void *entry);
	
#define DDK_IRQ_CONTROLLER_OPS_MASK		1
#define DDK_IRQ_CONTROLLER_OPS_UNMASK	2
#define DDK_IRQ_CONTROLLER_OPS_MASK_ACK 3
#define DDK_IRQ_CONTROLLER_OPS_DISABLE	4
	void (*irq_controller_ops)(int irq, int ops);
	
	/************************************************************************/
	/* PCI ������(��֤struct_pci_device_id����һ��)                           */
	/************************************************************************/
	void (*pci_link_irq_number)(void *(*link_method)(void *struct_pci_device_id, void *hal_device, int irq));
	int  (*pci_root_operation)(unsigned int domain, unsigned int bus_nr, unsigned int devfn, int where, int size, void *u32_val, int write);
	int  (*pci_bios_enable_device)(void *pdev, int mask);
	void (*pci_bios_disable_device)(void *pdev);

	/************************************************************************/
	/* Memory                                                               */
	/************************************************************************/
	void *(*mem_ioremap)(unsigned long phy, unsigned long size, unsigned long flags);

	/************************************************************************/
	/* Thread                                                               */
	/************************************************************************/

	/**
		@brief create a thread for driver system

		@return the thread object in kernel, NULL on failure
	*/
	void *(*create_thread)(void *ring0_stack_top, int stack_size, void *entry, unsigned long para);

	/**
		@brief Wakup a thread
	*/
	void (*wakeup_thread)(void *ko_thread);
	
	/**
		@brief Give up cpu for next thread
	*/
	void *(*yield_current_for)(void *pre_ko_thread, int pre_is_run, void *next_ko_thread);

	/**
		@brief DSS Idle
	*/
	void (*cpu_idle)(void);
	
	/************************************************************************/
	/* FS                                                                   */
	/************************************************************************/
	void (*fss_vfs_register)(struct fss_vfs_driver *driver);

	/**
		@brief ����DSS���ļ�ϵͳ����һ������Ļص�֪ͨ��Ŀǰ���Ƿ���Ϣ��DSSȥ��һ���ļ�������Ч�ʲ�һ�㣬û�취��˭������û�о���ȥ����ԭ���ļ�ϵͳ�أ�
	*/
	int (*fss_ops_wait)(void);

	/************************************************************************/
	/* NET                                                                  */
	/************************************************************************/
	void (*nss_hwmgr_register)(struct nss_hwmgr *nss);

	/************************************************************************/
	/* INPUT                                                                */
	/************************************************************************/
	void* (*input_register_handle)(void *drv_input_handle);

	/************************************************************************/
	/* misc                                                                 */
	/************************************************************************/
	void (*run_first_user_process)(void *data, int size, char *cmdline);
};
extern struct ddk_for_linux ddk;

/************************************************************************/
/* External Interface  declaration for making compiler happy            */
/************************************************************************/
void pci_link_irq_number(void *(*link_method)(void *struct_pci_device_id, void *hal_device, int irq));
int  pci_root_operation(unsigned int domain, unsigned int bus_nr, unsigned int devfn, int where, int size, void *u32_val, int write);
int  pci_bios_enable_device(void *pdev, int mask);
void pci_bios_disable_device(void *pdev);

#endif


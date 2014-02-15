/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   Interface for Driver Package
 */

/* 2个层次的接口一起用（导出和内部）*/

/* For ddk/vfs.h which cannot include this file */
#include <list.h>

#include <ddk/debug.h>
#include <ddk/irq.h>
#include <ddk/vfs.h>
#include <ddk/net.h>
#include <ddk/input.h>
#include <ddk/ddk_for_linux.h>

/* HAL internal interface */
#include <irq.h>
#include <irqdesc.h>

#include <kernel/ke_memory.h>

#include <thread.h>
#include <memory.h>
#include <process.h>
#include <cpu.h>

static void input_event(struct ddk_input_handle *handle, unsigned int event_type,
						unsigned int event_code, int value)
{
	switch(event_type)
	{
		case DDK_INPUT_EV_KEY:
			//是输入键还是弹起键
			if(value == 1)
				ifi_input_stream(IFI_DEV_STD_IN, &event_code, 1);
			break;
		default:
				//printk("type = %d,code = %d, value = %d\n", event_type, event_code, value);
			break;
	}
	return;	
	//printk("type = %x.\n", event_type);
	if (0)
	{
		extern struct ko_thread *tmsg;
		MSG_MAKE(2,0, 111);
		if (tmsg)
		{
			printk("Send result %d.\n", ktm_send(tmsg, pmsg));
		}
	}
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
static int ddk_printk(const char *fmt, ...)
{
	return 0;
}

/**
	@brief The driver need a physical space to run, we allocate and map it to its space
*/
static void allocate_physical_bulk(unsigned long *start, unsigned long *size, unsigned long map_base)
{
	struct ke_mem_cluster_info info;
	
	/* Allocate physical for driver system */
	if (NULL == km_cluster_alloc(&info, -1, false))
		goto err;
	else
	{
		*start = info.page_start;
		*size = info.page_count * PAGE_SIZE;
	}
	
	/* Map to the virtual address space of driver system to access */
	if (NULL == km_map_physical(info.page_start, *size,
					(map_base + info.page_start) |  KM_MAP_PHYSICAL_FLAG_WITH_VIRTUAL | KM_MAP_PHYSICAL_FLAG_NORMAL_CACHE))
	{
		goto err1;
	}
	return;
	
err1:
	km_cluster_dealloc_base(info.page_start, true);
	
err:
	*start = 0;
	*size = 0;
}

static void setup_irq_handler(void *entry)
{
	hal_setup_external_irq_handler(entry);
}

static void irq_controller_ops(int irq, int ops)
{
	struct irq_desc *desc = irq_to_desc(irq);
	
	switch (ops)
	{
		case DDK_IRQ_CONTROLLER_OPS_MASK:
			if (desc->irq_data.chip->irq_mask)
				desc->irq_data.chip->irq_mask(&desc->irq_data);
			break;
		case DDK_IRQ_CONTROLLER_OPS_DISABLE:
			if (desc->irq_data.chip->irq_disable)
				desc->irq_data.chip->irq_disable(&desc->irq_data);
			break;
		case DDK_IRQ_CONTROLLER_OPS_UNMASK:
			if (desc->irq_data.chip->irq_unmask)
				desc->irq_data.chip->irq_unmask(&desc->irq_data);
			break;
		case DDK_IRQ_CONTROLLER_OPS_MASK_ACK:
			if (desc->irq_data.chip->irq_mask_ack)
				desc->irq_data.chip->irq_mask_ack(&desc->irq_data);
			else
			{
				desc->irq_data.chip->irq_mask(&desc->irq_data);
				if (desc->irq_data.chip->irq_ack)
					desc->irq_data.chip->irq_ack(&desc->irq_data);
			}
			break;
	}
}

static void *create_thread(void *ring0_stack, int stack_size, void *entry, unsigned long para)
{
	struct ko_thread *p = kt_create_driver_thread(ring0_stack, stack_size, entry, para);
	
	/* Set ti of this thread */
	p->desc_of_driver = (void*)((unsigned long)ring0_stack & PAGE_MASK);
	return p;
}

static void wakeup_thread(void *ko_thread)
{
	struct ko_thread *k = ko_thread;
	kt_wakeup_driver(k);
}

static void *yield_current_for(void *pre_ko_thread, int pre_is_run, void *next_ko_thread)
{
	struct ko_thread *next = next_ko_thread;

#if 0
	struct ko_thread *pre = pre_ko_thread, 
	printk("pre_ko_thread(%d) = %x,  next_ko_thread = %x.\n",
		   pre_is_run, pre_ko_thread, next_ko_thread);
#endif
	/* 
		Check the status transaction as managed by the driver subsystem.
		We have to reflect the thread status to the kernel.
		
		特别注意，我们此时模拟了CPU上下文切换的，并且是在关中断情况下。
		新换新的线程一定要比以前的线程先运行，比如idle thread1 thread2的yield顺序和实际上执行循序是一致的。
	*/
	kt_wakeup_driver(next);
	
	kt_schedule_driver();

	return NULL;
}

static void goto_idle()
{
	kt_schedule();	
}

static void *mem_ioremap(unsigned long phy, unsigned long size, unsigned long flags)
{
	/* Note: flags are flags in the name of this system */
	return km_map_physical(phy, size, flags);
}

/**
	@brief Wait fss in dds
*/
static int fss_ops_wait()
{
	/* Just yield current thread, giving other a chance */
	kt_schedule();

	//TODO: test thread dieing...
	return 0;
}

static void *input_register_handle(void *drv_input_handle)
{
	struct ddk_input_handle *handle = km_valloc(sizeof(*handle));
	if (!handle)
		return NULL;

	handle->drv_handle	= drv_input_handle;
	handle->event		= input_event;
}

struct ddk_for_linux ddk = {
	.printk					= ddk_printk,
	.allocate_physical_bulk = allocate_physical_bulk,
	
	.setup_irq_handler		= setup_irq_handler,
	.irq_controller_ops		= irq_controller_ops,

	.mem_ioremap			= mem_ioremap,

	.create_thread			= create_thread,
	.wakeup_thread			= wakeup_thread,
	.yield_current_for		= yield_current_for,
	.cpu_idle				= goto_idle,

	/* PCI */
	.pci_link_irq_number	= pci_link_irq_number,
	.pci_root_operation		= pci_root_operation, 
	.pci_bios_enable_device = pci_bios_enable_device,
	.pci_bios_disable_device= pci_bios_disable_device,
	
	/* DSS's File Operation */
	.fss_vfs_register		= fss_vfs_register,
	.fss_ops_wait			= fss_ops_wait,

	/* DSS's NSS */
	.nss_hwmgr_register		= nss_hwmgr_register,

	/* INPUT */
	.input_register_handle	= input_register_handle,

	/* MISC */
	.run_first_user_process = ke_run_first_user_process,
};
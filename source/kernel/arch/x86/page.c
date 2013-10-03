//#include <compiler.h>

#include <process.h>
#include <thread.h>
#include <section.h>
#include <walk.h>
#include <memory.h>

#include <asm/system.h>
#include <asm/desc.h>
#include <asm/ptrace.h>

extern void *hal_x86_get_init_pgtable();

static bool address_in_user(unsigned long address)
{
	if (address < HAL_GET_BASIC_KADDRESS(0))
		return true;
	return false;
}

static void copy_kernel_to_user(struct km * mm_ctx, int base, int end)
{
	unsigned long *src, *dst;
	int i;

	/* Copy the entry from kernel to dst */
	src = hal_x86_get_init_pgtable();
	dst = mm_ctx->translation_table;
	for (i = base; i < end; i++)
		dst[i] = src[i] & (~PAGE_FLAG_USER);
}

/**	
	@brief Page fault handler
*/
asmregparm void do_page_fault(struct pt_regs * regs, long error_code)
{
	unsigned long error_address = read_cr2();	
	bool in_user = address_in_user(error_address);
	struct ko_thread *current;	 

	//printk("PAGE fault: eip %x addr %x code %x\n", regs->ip, error_address, error_code);
	/*
		Access violation.
		读、写和u/s异常
		如果是用户模式，那么访问核心层空间时崩溃，其他地方尝试修复
		如果是特权模式，都尝试修复
	*/
	if (error_code & PAGE_FAULT_U)
	{
		if (in_user == false)
			goto die_cur;
	}
	
	if (!in_user)
		error_code |= PAGE_FAULT_IN_KERNEL;
	current = kt_current();
	if (unlikely(false == ks_exception(current, error_address, error_code)))
		goto die_cur;

	return;	

die_cur:
	printk("TODO: Thread deing :eip %x addr %x code %x\n", regs->ip, error_address, error_code);

	//TODO kill thread
	//fak_arch_x86_dump_register(regs);
	kt_delete_current();
}

__init void km_arch_trim()
{
	unsigned long i;
	
	unsigned long *p = hal_x86_get_init_pgtable();
	
	/*
	 Kernel part in 4mb page;
		1,低端。
		2,高端
	*/
	for(i = 0; i < HAL_GET_BASIC_KADDRESS(0) / 0x400000; i++)
		p[i] = 0;																//Mask as invalid;
	for(i = (HAL_GET_BASIC_KADDRESS(0) + CONFIG_HAL_KERNEL_MEM_LEN) / 0x400000; i < 1024; i++)
		p[i] = 0;																//Mask as invalid;
	
	write_cr3((unsigned long)hal_x86_get_init_pgtable() - HAL_GET_BASIC_KADDRESS(0));

	/*
	 load tr
	 */
	set_tss_desc(0, &init_tss);
	load_TR_desc();
	
	/*
	 打开WP功能
	 */
	do {
		unsigned long i = read_cr0();
		//if(open)
		i |= 1 << 16;
		//else
		//	i &= ~(1 << 16);
		write_cr0(i);
	} while (0);
}

void km_arch_init_for_kernel(struct km *mem)
{
	mem->translation_table = hal_x86_get_init_pgtable();
}

/**
	@brief Arch ctx init
*/
void km_arch_ctx_init(struct km * mm_ctx)
{
	/* Share the core's address space */
	int base = HAL_GET_BASIC_KADDRESS(0) / 0x400000;
	int end  = ARCH_KM_LV2_COUNT;

	copy_kernel_to_user(mm_ctx, base, end);
}

void km_arch_ctx_deinit(struct km * mm_ctx)
{
	/* Nothing for x86 */
}

/**
	@brief Switch memory context
*/
void km_arch_ctx_switch(struct km * pre_ctx, struct km * next_ctx)
{
	unsigned long cr3_physical;

	/* Get physical base of cr3 */
	cr3_physical = HAL_GET_BASIC_PHYADDRESS(next_ctx->translation_table);
	//TODO: not use this macro

	/* Write to cr3 to switch */
	write_cr3(cr3_physical);

#if 0
	/*load the LDT, if the LDT is different*/
	if(unlikely(pre_ctx->ldt!=new_ctx->ldt))
	{
		//printk("\nnew_ctx->ldt :%h,%d.",new_ctx->ldt,new_ctx->ldt_entries);
		set_ldt(new_ctx->ldt,new_ctx->ldt_entries);
	}
#endif
}

#include <compiler.h>

#include <walk.h>

#include <asm/system.h>
#include <asm/desc.h>

static void (*ret)() ;
extern void *hal_x86_get_init_pgtable();

__init void km_arch_trim()
{
	unsigned long i;
	
	unsigned long *p = hal_x86_get_init_pgtable();
	ret = __builtin_return_address(0);
	
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
	ret();
}

void km_arch_init_for_kernel(struct km *mem)
{
	mem->translation_table = hal_x86_get_init_pgtable();
}
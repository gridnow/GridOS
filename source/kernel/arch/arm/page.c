/**
	Page support for ARM
 
	Wuxin
	82828068@qq.com
*/


#include <process.h>
#include <thread.h>
#include <section.h>
#include <walk.h>
#include <memory.h>

/**
	@brief Arch ctx init
 */
void km_arch_ctx_init(struct km * mm_ctx)
{
#if 0
	/* Share the core's address space */
	int base = HAL_GET_BASIC_KADDRESS(PHYS_OFFSET) / 0x400000;
	int end  = ARCH_KM_LV2_COUNT;
	
	copy_kernel_to_user(mm_ctx, base, end);
#endif
	TODO("");
}

void km_arch_init_for_kernel(struct km *mem)
{
	mem->translation_table = hal_arm_get_init_pgtable();
}

__init void km_arch_trim()
{
	// ARM no need to trim, we just map the initial directly memory at HAL setup
}
#include <types.h>
#include <ddk/debug.h>

#include <asm/cacheflush.h>
#include <asm/processor.h>
#include <asm/cpu.h>
#include <asm/cpu-features.h>

/* CPU 的默认缓存策略, HAL只是记录，用于内部配置 */	
unsigned long _page_cachable_default;

/* CPU 的 CACHEops, 虽然目前只有一个，以后可能每个CPU都有自己的 CACHE OPS */
struct mips_cache_local_ops cache_local_ops;

// /* Cache operations. */
// void (*flush_cache_all)(void);
// void (*__flush_cache_all)(void);
// void (*flush_icache_range)(unsigned long start, unsigned long end);
// void (*local_flush_icache_range)(unsigned long start, unsigned long end);
// 
// void (*__flush_cache_vmap)(void);
// void (*__flush_cache_vunmap)(void);
// 
// /* MIPS specific cache operations */
// void (*flush_cache_sigtramp)(unsigned long addr);
// void (*local_flush_data_cache_page)(void * addr);
// void (*flush_data_cache_page)(unsigned long addr);
// void (*flush_icache_all)(void);
void __cpuinit cpu_cache_init(void)
{
	if (cpu_has_4k_cache) {
		extern void __weak r4k_cache_init(void);

		r4k_cache_init();
	}
	else
	{
		TODO("");
	}
}


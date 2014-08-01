#ifndef _ASM_CACHEFLUSH_H
#define _ASM_CACHEFLUSH_H

/* Keep includes the same across arches.  */
#include <asm/cpu-features.h>

/* Run kernel code uncached, useful for cache probing functions. */
unsigned long run_uncached(void *func);

/* ֻ��Ϊ�����ã����ﲻ�����ڴ���� */

extern unsigned long _page_cachable_default;

/************************************************************************/
/* CACHE �ӿڲ�                                                         */
/************************************************************************/
struct mips_cache_local_ops
{
	void (*flush_all)();
	void (*flush_icache_range)(unsigned long start, unsigned long end);
};
extern struct mips_cache_local_ops cache_local_ops;
#define local_flush_icache_range cache_local_ops.flush_icache_range

#endif
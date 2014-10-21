#include <ddk/compiler.h>
#include <types.h>
#include <smp.h>

#include <asm/cpu-info.h>
#include <asm/cpu.h>

/* Return the scache size for c-r4k */
int loongson2_sc_init(void)
{
	struct cpuinfo_mips *c = &current_cpu_data;
	int scache_size;
	
	scache_size = 512*1024;
	c->scache.linesz = 32;
	c->scache.ways = 4;
	c->scache.waybit = 0;
	c->scache.waysize = scache_size / (c->scache.ways);
	c->scache.sets = scache_size / (c->scache.linesz * c->scache.ways);

	c->options |= MIPS_CPU_INCLUSIVE_CACHES;
	
	return scache_size;
}

static void loongson3_flush_all()
{	
}

static void loongson2_flush_all()
{
	r4k_blast_scache();
}

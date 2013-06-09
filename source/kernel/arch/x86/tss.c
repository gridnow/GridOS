#include <stddef.h>
#include <asm/processor.h>
#include <asm/desc.h>

#ifdef CONFIG_X86_32
/*
 * Note that the .io_bitmap member must be extra-big. This is because
 * the CPU will access an additional byte beyond the end of the IO
 * permission bitmap. The extra byte must be all 1 bits, and must
 * be within the limit.
 */
#define FAK_INIT_TSS  {							  \
	.x86_tss = {							  \
	.sp0		= 0/*切换线程时会保存的*/, \
	.ss0		= __KERNEL_DS,				  \
	.ss1		= __KERNEL_CS,				  \
	.io_bitmap_base	= INVALID_IO_BITMAP_OFFSET,		  \
},								  \
	.io_bitmap		= { [0 ... IO_BITMAP_LONGS] = ~0 },	  \
}
#else

#endif

/*
 * per-CPU TSS segments. Threads are completely 'soft' on Linux,
 * no more per-task TSS's. The TSS size is kept cacheline-aligned
 * so they are allowed to end up in the .data..cacheline_aligned
 * section. Since TSS's are completely CPU-local, we want them
 * on exact cacheline boundaries, to eliminate cacheline ping-pong.
 */
DEFINE_PER_CPU_SHARED_ALIGNED(struct tss_struct, init_tss) = FAK_INIT_TSS;


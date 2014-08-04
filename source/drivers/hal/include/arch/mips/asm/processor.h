#ifndef _ASM_PROCESSOR_H
#define _ASM_PROCESSOR_H

#include <asm/cpu.h>
#include <asm/cpu-info.h>
#include <asm/mipsregs.h>
#include <asm/system.h>
#include <compiler.h>

#define NUM_FPU_REGS	32
typedef u64 fpureg_t;
/*
 * It would be nice to add some more fields for emulator statistics, but there
 * are a number of fixed offsets in offset.h and elsewhere that would have to
 * be recalculated by hand.  So the additional information will be private to
 * the FPU emulator for now.  See asm-mips/fpu_emulator.h.
 */
struct mips_fpu_struct {
	fpureg_t	fpr[NUM_FPU_REGS];
	unsigned int	fcr31;
};

#define NUM_DSP_REGS   6

typedef __u32 dspreg_t;

struct mips_dsp_state {
	dspreg_t	dspr[NUM_DSP_REGS];
	unsigned int	dspcontrol;
};

struct mips3264_watch_reg_state {
	/* The width of watchlo is 32 in a 32 bit kernel and 64 in a
	   64 bit kernel.  We use unsigned long as it has the same
	   property. */
	unsigned long watchlo[NUM_WATCH_REGS];
	/* Only the mask and IRW bits from watchhi. */
	u16 watchhi[NUM_WATCH_REGS];
};

union mips_watch_reg_state {
	struct mips3264_watch_reg_state mips3264;
};

/*
 * Return_address is a replacement for __builtin_return_address(count)
 * which on certain architectures cannot reasonably be implemented in GCC
 * (MIPS, Alpha) or is unusable with -fomit-frame-pointer (i386).
 * Note that __builtin_return_address(x>=1) is forbidden because GCC
 * aborts compilation on some CPUs.  It's simply not possible to unwind
 * some CPU's stackframes.
 *
 * __builtin_return_address works only for non-leaf functions.  We avoid the
 * overhead of a function call by forcing the compiler to save the return
 * address register on the stack.
 */
#define return_address() ({__asm__ __volatile__("":::"$31");__builtin_return_address(0);})

#ifdef CONFIG_CPU_HAS_PREFETCH

#define ARCH_HAS_PREFETCH
#define prefetch(x) __builtin_prefetch((x), 0, 1)

#define ARCH_HAS_PREFETCHW
#define prefetchw(x) __builtin_prefetch((x), 1, 1)

#endif

#define cpu_relax()	barrier()


#endif /* _ASM_PROCESSOR_H */

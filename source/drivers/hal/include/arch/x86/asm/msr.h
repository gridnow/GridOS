#ifndef X86_MSR_H
#define X86_MSR_H
#include <asm/msr-index.h>

/*
* both i386 and x86_64 returns 64-bit value in edx:eax, but gcc's "A"
* constraint has different meanings. For i386, "A" means exactly
* edx:eax, while for x86_64 it doesn't mean rdx:rax or edx:eax. Instead,
* it means rax *or* rdx.
*/
#ifdef CONFIG_X86_64
#define DECLARE_ARGS(val, low, high)	unsigned low, high
#define EAX_EDX_VAL(val, low, high)	((low) | ((u64)(high) << 32))
#define EAX_EDX_ARGS(val, low, high)	"a" (low), "d" (high)
#define EAX_EDX_RET(val, low, high)	"=a" (low), "=d" (high)
#else
#define DECLARE_ARGS(val, low, high)	unsigned long long val
#define EAX_EDX_VAL(val, low, high)	(val)
#define EAX_EDX_ARGS(val, low, high)	"A" (val)
#define EAX_EDX_RET(val, low, high)	"=A" (val)
#endif

static inline unsigned long long native_read_msr(unsigned int msr)
{
	DECLARE_ARGS(val, low, high);

	asm volatile("rdmsr" : EAX_EDX_RET(val, low, high) : "c" (msr));
	return EAX_EDX_VAL(val, low, high);
}
static inline void native_write_msr(unsigned int msr,
									unsigned low, unsigned high)
{
	asm volatile("wrmsr" : : "c" (msr), "a"(low), "d" (high) : "memory");
}

static __always_inline unsigned long long __native_read_tsc(void)
{
	DECLARE_ARGS(val, low, high);

	asm volatile("rdtsc" : EAX_EDX_RET(val, low, high));

	return EAX_EDX_VAL(val, low, high);
}

static inline unsigned long long native_read_pmc(int counter)
{
	DECLARE_ARGS(val, low, high);

	asm volatile("rdpmc" : EAX_EDX_RET(val, low, high) : "c" (counter));
	return EAX_EDX_VAL(val, low, high);
}

/*
* Access to machine-specific registers (available on 586 and better only)
* Note: the rd* operations modify the parameters directly (without using
* pointer indirection), this allows gcc to optimize better
*/

#define rdmsr(msr, val1, val2)					\
	do {								\
	u64 __val = native_read_msr((msr));			\
	(void)((val1) = (u32)__val);				\
	(void)((val2) = (u32)(__val >> 32));			\
	} while (0)

static inline void wrmsr(unsigned msr, unsigned low, unsigned high)
{
	native_write_msr(msr, low, high);
}

#define rdmsrl(msr, val)			\
	((val) = native_read_msr((msr)))

#define wrmsrl(msr, val)						\
	native_write_msr((msr), (u32)((u64)(val)), (u32)((u64)(val) >> 32))

#define rdtscl(low)						\
	((low) = (u32)__native_read_tsc())

#define rdtscll(val)						\
	((val) = __native_read_tsc())

#endif

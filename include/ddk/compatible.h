/*
	OLD driver need this
*/
#ifndef _DDK_COMPATIBLE_INTERFACE_
#define _DDK_COMPATIBLE_INTERFACE_
#include <compiler.h>
#include <list.h>

/*************************************************************/
/* Memory */
/*************************************************************/
inline static void * ke_vm_basic(unsigned long basic_physical)
{
	/*  将基本物理内存（一般是低端内存）转换成内核、驱动能直接访问的虚拟地址*/
	void *p = (void*)(basic_physical + 0x80000000UL);
	//TODO
	return p;
}
#define __va(phy) ke_vm_basic(phy)
#if 0
struct page;									/* 模拟一个假的,其实是页的起始虚拟地址,后续的转换要注意 */
#define virt_to_page(v) v						/* 虚拟地址转换到页地址，在hal中就是虚拟地址，hal的虚拟是连续的，物理地址也是连续的 */
#define page_to_phys(p) 0						/* 页地址（其实就是虚拟地址）到物理地址的转换，如果转后的物理地址要进行跨页DMA，调用者保证该物理地址来自HAL（也即是连续的）*/
DLLEXPORT void hal_wmb();
DLLEXPORT void hal_rmb();
#endif

/*************************************************************/
/* LOCK for drivers */
/*************************************************************/
#include <kernel/ke_lock.h>
typedef struct
{
	struct ke_spinlock lock;
}raw_spinlock_t;
#ifndef SPINLOCK_T
typedef raw_spinlock_t spinlock_t;
#define SPINLOCK_T
#endif

#define raw_spin_lock_irqsave(L, flags) flags = ke_spin_lock_irqsave(&((L)->lock))
#define raw_spin_unlock_irqrestore(L, flags) ke_spin_unlock_irqrestore(&((L)->lock), flags)
#define raw_spin_lock(L) ke_spin_lock(&((L)->lock))
#define raw_spin_unlock(L) ke_spin_unlock(&((L)->lock))
#define raw_spin_lock_init(L) ke_spin_init(&((L)->lock))

#define spin_lock_init(L) ke_spin_init((struct ke_spinlock*)L)
#define spin_lock(L) ke_spin_lock((struct ke_spinlock*)L)
#define spin_unlock(L) ke_spin_unlock((struct ke_spinlock*)L)
#define spin_lock_irq(L) ke_spin_lock_irq((struct ke_spinlock*)L)
#define spin_unlock_irq(L) ke_spin_unlock_irq((struct ke_spinlock*)L)
#define spin_lock_irqsave(L, flags) flags = ke_spin_lock_irqsave((struct ke_spinlock*)L)
#define spin_unlock_irqrestore(L, flags) ke_spin_unlock_irqrestore((struct ke_spinlock*)L, flags)
#define DEFINE_RAW_SPINLOCK(x)	raw_spinlock_t x = {{0}} //TODO: 不同的ARCH，计数器初始化未必一样
#define DEFINE_SPINLOCK(x) struct ke_spinlock x = {0}//TODO: 不同的ARCH，计数器初始化未必一样

/************************************************************************
 Device
*************************************************************************/
struct device
{
	struct list_head devres_head;
	struct ke_spinlock devres_lock;
	u64 *dma_mask;
};

typedef void (*dr_release_t)(struct device *dev, void *res);
typedef int (*dr_match_t)(struct device *dev, void *res, void *match_data);
DLLEXPORT void * dr_devres_find(struct device *dev, dr_release_t release,
				   dr_match_t match, void *match_data);
#define devres_find dr_devres_find

/************************************************************************
 Soft Timer
*************************************************************************/
#define TIMER_NOT_PINNED	0
#define TIMER_PINNED		1

#define TIMER_DEFERRABLE		0x1LU
#define TIMER_IRQSAFE			0x2LU

#define TIMER_FLAG_MASK			0x3LU

struct tvec_base;
struct timer_list {
	/*
	 * All fields that change during normal runtime grouped to the
	 * same cacheline
	 */
	struct list_head entry;
	unsigned long expires;
	struct tvec_base *base;
	
	void (*function)(unsigned long);
	unsigned long data;
	
	int slack;
};

DLLEXPORT int hal_timer_mod(struct timer_list *timer, unsigned long expires);
DLLEXPORT void hal_timer_init(struct timer_list *timer, unsigned int flags, const char *name);
DLLEXPORT void hal_timer_add(struct timer_list *timer);
DLLEXPORT int hal_timer_del(struct timer_list *timer);
DLLEXPORT unsigned long hal_get_tick();
DLLEXPORT unsigned int hal_get_tick_rate();
DLLEXPORT unsigned long hal_ms_to_tick(int ms);

/* MISC */
#define EXPORT_SYMBOL_GPL(X)

/************************************************************************
 schedule
*************************************************************************/
#include <kernel/kernel.h>
static inline signed long schedule_timeout_uninterruptible(signed long timeout)
{
	return ke_sleep_timeout(timeout);
}

#endif

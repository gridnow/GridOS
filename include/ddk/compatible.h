/*
	OLD driver need this
*/
#ifndef DDK_COMPATIBLE_INTERFACE
#define DDK_COMPATIBLE_INTERFACE

/* Memory */
#define GFP_KERNEL 0
static inline void *kzalloc(size_t size, int flags)
{
	return NULL;
}

static void inline kfree(void *p)
{
	
}

/* LOCK */
typedef struct __raw_spinlock
{
	struct ke_spinlock lock;
} raw_spinlock_t;
#define raw_spin_lock_irqsave(L, flags) flags = ke_spin_lock_irqsave(&((L)->lock))
#define raw_spin_unlock_irqrestore(L, flags) ke_spin_unlock_irqrestore(&((L)->lock), flags)
#define raw_spin_lock(L) ke_spin_lock(&((L)->lock))
#define raw_spin_unlock(L) ke_spin_unlock(&((L)->lock))
#define raw_spin_lock_init(L) ke_spin_init(&((L)->lock))
#define spin_lock(L) ke_spin_lock(&((L)->lock))
#define spin_unlock(L) ke_spin_unlock(&((L)->lock))
#define spin_lock_irq(L) ke_spin_lock_irq(&((L)->lock))
#define spin_unlock_irq(L) ke_spin_unlock_irq(&((L)->lock))

#define __RAW_SPIN_LOCK_UNLOCKED(lockname)	\
	(raw_spinlock_t) __RAW_SPIN_LOCK_INITIALIZER(lockname)
#define DEFINE_RAW_SPINLOCK(x)	raw_spinlock_t x = __RAW_SPIN_LOCK_UNLOCKED(x)
#endif

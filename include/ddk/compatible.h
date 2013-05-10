/*
	OLD driver need this
*/
#ifndef _DDK_COMPATIBLE_INTERFACE_
#define _DDK_COMPATIBLE_INTERFACE_

#include <list.h>

/* Memory */
inline static void * ke_vm_basic(unsigned long basic_physical)
{
	/*  �����������ڴ棨һ���ǵͶ��ڴ棩ת�����ںˡ�������ֱ�ӷ��ʵ������ַ*/
	void *p = (void*)(basic_physical + 0x80000000UL);
	//TODO
	return p;
}
#define __va(phy) ke_vm_basic(phy)

/* LOCK */
typedef struct __raw_spinlock
{
	struct ke_spinlock lock;
} raw_spinlock_t;
typedef struct ke_spinlock spinlock_t;

#define raw_spin_lock_irqsave(L, flags) flags = ke_spin_lock_irqsave(&((L)->lock))
#define raw_spin_unlock_irqrestore(L, flags) ke_spin_unlock_irqrestore(&((L)->lock), flags)
#define raw_spin_lock(L) ke_spin_lock(&((L)->lock))
#define raw_spin_unlock(L) ke_spin_unlock(&((L)->lock))
#define raw_spin_lock_init(L) ke_spin_init(&((L)->lock))
#define spin_lock(L) ke_spin_lock(L)
#define spin_unlock(L) ke_spin_unlock(L)
#define spin_lock_irq(L) ke_spin_lock_irq(L)
#define spin_unlock_irq(L) ke_spin_unlock_irq(L)
#define spin_lock_irqsave(L, flags) flags = ke_spin_lock_irqsave(L)
#define spin_unlock_irqrestore(L, flags) ke_spin_unlock_irqrestore(L, flags)
#define DEFINE_RAW_SPINLOCK(x)	raw_spinlock_t x = {{0}} //TODO: ��ͬ��ARCH����������ʼ��δ��һ��
#define DEFINE_SPINLOCK(x) spinlock_t x = {0}//TODO: ��ͬ��ARCH����������ʼ��δ��һ��

/************************************************************************
 Device
*************************************************************************/
struct device
{
	struct list_head devres_head;
	spinlock_t devres_lock;
};

typedef void (*dr_release_t)(struct device *dev, void *res);
typedef int (*dr_match_t)(struct device *dev, void *res, void *match_data);
DLLEXPORT void * dr_devres_find(struct device *dev, dr_release_t release,
				   dr_match_t match, void *match_data);
#define devres_find dr_devres_find

/************************************************************************
 NET Device
 *************************************************************************/
#define alloc_etherdev(sizeof_priv) alloc_etherdev_mq(sizeof_priv, 1)
#define alloc_etherdev_mq(sizeof_priv, count) alloc_etherdev_mqs(sizeof_priv, count, count)

/* MISC */
#define EXPORT_SYMBOL_GPL(X)


#endif

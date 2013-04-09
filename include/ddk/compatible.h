/*
	OLD driver need this
*/
#ifndef DDK_COMPATIBLE_INTERFACE
#define DDK_COMPATIBLE_INTERFACE

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

/* Device */
struct device
{
	int dummy;
};

/* MISC */
#define EXPORT_SYMBOL_GPL(X)

#endif

#include "atomic.h"
#include <kernel/ke_atomic.h>

/** 
	@brief ԭ�ӻ�����������1
*/
void ke_atomic_inc(struct ke_atomic * atomic)
{
	atomic_inc((atomic_t*)atomic);
}

/**
	@brief ԭ�ӻ�����������1
*/
void ke_atomic_dec(struct ke_atomic * atomic)
{
	atomic_dec((atomic_t*)atomic);
} 

/**
	@brief ��ȡ������
*/
int ke_atomic_read(struct ke_atomic * atomic)
{
	return atomic_read((atomic_t*)atomic);
}

/**
	@brief ����һ��ԭ�Ӽ�����
*/
void ke_atomic_set(struct ke_atomic * atomic, int count)
{
	atomic_set((atomic_t*)atomic, count);
}

/**
	@brief ����һ��ԭ�Ӽ�����
*/
int ke_atomic_add_return(struct ke_atomic * atomic, int count)
{
	return atomic_add_return(count, (atomic_t*)atomic);
}

int ke_atomic_sub_return(struct ke_atomic * atomic, int count)
{
	return atomic_sub_return(count, (atomic_t*)atomic);
}


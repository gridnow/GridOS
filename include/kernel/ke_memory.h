/**
    @defgroup memory
    @ingroup kernel_api

    �ں˱�̽ӿ� �ڴ��������

    @{
*/

#ifndef KE_MEMORY_H
#define KE_MEMORY_H

#include <compiler.h>
#include "kernel.h"

/**
	@brief Map IO memory space
*/
DLLEXPORT void *km_map_physical(unsigned long physical, unsigned long size, unsigned long flags);
#define KM_MAP_PHYSICAL_FLAG_WITH_VIRTUAL	(1 << 0)				/* �����ַ�󶨵��ض������ַ, �����ַ����PAGE_SIZE���� */
#define KM_MAP_PHYSICAL_FLAG_NORMAL_CACHE	(1 << 1)				/* �����ַӳ��ʱ��ʹ�ó����cacheģʽ�������Ƿǻ��棩*/

/**
	@brief �����ں�̬ȫ�ֳ����ڴ�ռ��
*/
DLLEXPORT void *km_alloc_virtual(unsigned long size, page_prot_t prot);

/**
	@brief �ͷ��ں�ȫ�ֳ����ڴ�ռ��
*/
DLLEXPORT void km_dealloc_virtual(void *kv);

/**
	@brief Allocate a kernel page
*/
DLLEXPORT void *km_alloc_kernel_page();

/**
	@brief �����ں�̬ȫ�ֹ��������ڴ�ռ�
*/
DLLEXPORT void *km_valloc(unsigned long size);

/**
	@brief �ͷ��ں�ȫ�ֹ��������ڴ�ռ�
*/
DLLEXPORT void *km_vfree(void *kp);

#endif

/** @} */

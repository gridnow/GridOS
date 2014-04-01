
#ifndef _RING_BUFF_H
#define _RING_BUFF_H

#include <types.h>

/* ����package����ͷ��Ч���� */
#define INVALID_FLAG  0
#define VALID_FLAG 1
/* ����ʹ���б�־,��ֹ�ٴζ�ȡ */
#define BUSY_FLAG  2

/* �����߱�֤���̷߳��ʰ�ȫ */
struct ring_buff_cache
{
	/* ����buff cache ��С */
	size_t cache_buff_length;
	/* �ɶ�buff cache���ݵĵ�ַ,��ֵӦ����ʵ�����ݵĿ�ʼ�� */
	unsigned long curr_read_pos;
	
	unsigned long curr_write_pos;

	/* �Ƿ��ʼ����־ */
	int is_init;
};

struct ring_package
{
	/* ��flag��ʾ�ñ����Ƿ�ռ�� 1 ռ�� 0 �ͷ� 2 Э��ջռ��*/
	int valid_flag;
	/* ��һ����ͷ��λ��,Ϊ0��ʾ�ð�ͷΪ���  */
	unsigned long next_head;
	/* ǰһ����ͷλ��,Ϊ0��ʾ�ð�ͷΪ��ǰ */
	unsigned long prev_head;
	/* ����λ�� */
	unsigned long package_offset;
	/* ����С */
	size_t package_size;
};

/**
	@brief ����һ���ڴ�����,��ʼ��Ϊring buff
	@return
		cache head �ɹ�   NULL ʧ��
*/
struct ring_buff_cache * ring_buff_head_init(void *cache, size_t length);

/**
	@brief ����һ������ͷ
	@return
		�ɹ����ر���body  ʧ�ܷ��� NULL
*/
void *ring_buff_alloc(struct ring_buff_cache *cache, size_t length);

/**
	@brief �ͷ�buff ���ϲ����ڿ���buff
	@return 
		��
*/
void ring_buff_free(struct ring_buff_cache *cache, void *buff);

/**
	@brief ��ȡ��ǰ�ɶ�ȡ����ͷ
	@return
		�пɶ����ķ���package  �޷��� NULL
*/
struct ring_package * ring_cache_read_package(struct ring_buff_cache *cache);

/**
	@brief ����ʹ��,��ӡ���������е�package head��Ϣ
	@return 
		��
*/
void cache_package_head_info_debug(struct ring_buff_cache *cache);

#endif






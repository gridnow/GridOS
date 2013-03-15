/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   wuxin
*   λͼ����
*/
#ifndef COMMON_LIB_BITMAP_H
#define COMMON_LIB_BITMAP_H
#include <types.h>
#include <limits.h>

struct cl_bitmap
{	
	unsigned long *bitmap;
	unsigned long mask_count;
	unsigned long last_bit;	
};

/**
	@brief ����bitmap�����bitmapλͼ����ֻ����bitmapλͼ

	����cur_bitmap�Ƿ�ΪNULL���ж��Ƿ񴴽�bitmap���� TRUE�򴴽���FALSE�򲻴���

	@param[in] cur_bitmap �����BITMAP������
	@param[in] bitmap �������׵�ַ
	@param[in] mask_count  λͼ������λ�ĸ���
*/
void cl_bitmap_init(struct cl_bitmap *cur_bitmap, unsigned long *bitmap, unsigned long mask_count);

/**
	@brief �ڻ�������ӦBITMAP�Ĵ�С

	�û���ʱֻ֪����������ÿ���С����Ҫ�Զ�����BITMAP�ߴ�

	@param[in] bitmap �����BITMAP������
	@param[in] buffer �������׵�ַ
	@param[in] buffer_size �������ߴ�
	@param[in] block_size ������ƽ�ֵ�ÿ���С��������ܱ�sizeof(unsigned long)����������������false

	@return �ɹ�����true��ʧ�ܷ���false��ʧ����Ҫ�����ڴ���Ĳ��������ʡ�
*/
bool cl_bitmap_adaption(struct cl_bitmap *bitmap, void *buffer, int buffer_size, int block_size);

/**
	@brief ����һ��λ

	��bitmap�з���һ��λ�󷵻�

	@param[in] cur_bitmap �����BITMAP������

	@return �ɹ�����λ��ţ�ʧ���򷵻� HAL_BITMAP_ALLOC_BIT_ERROR
*/
unsigned long cl_bitmap_alloc_bit(struct cl_bitmap *cur_bitmap);
#define HAL_BITMAP_ALLOC_BIT_ERROR -1UL

/**
	@brief ��������λ

	��bitmap�з���һ��λ��λ��λ֮�������������ж�

	@param[in] cur_bitmap �����BITMAP������
	@param[in] count  Ҫ������ٸ�λ��

	@return �ɹ��󷵻��ױ�ţ�ʧ���򷵻�HAL_BITMAP_ALLOC_BIT_ERROR
*/
unsigned long cl_bitmap_alloc_consistant_bits(struct cl_bitmap *cur_bitmap, int count);

/**
	@brief �ͷ�һ��λ

	��cur_bitmap���ͷ�һ��λ

	@param[in] cur_bitmap �����BITMAP������
	@param[in] bit Ҫ�ͷŵ�λ�ı��

	@return �ɹ����� true��ʧ�ܷ��� false
*/
bool cl_bitmap_dealloc_bit(struct cl_bitmap *cur_bitmap, unsigned long bit);

/**
	@brief �ͷ�������BIT

	��bitmap���ͷ�һ��λ��λ��λ֮�������������ж�

	@param[in] cur_bitmap �����BITMAP������
	@param[in] count Ҫ�ͷŵ�����
	@param[in] bit Ҫ�ͷŵ�λ����ʼ���
*/
unsigned long cl_bitmap_dealloc_consistent_bits(struct cl_bitmap *cur_bitmap, int count, unsigned long bits);

/**
	@brief ��ȡbitmap�ֽڴ�С
	
	��ȡbitmap���������ٵ�λ���ֽڳߴ�

	@param[in] bitmap �����BITMAP������

	@return The size of the bitmap mask
*/
#define cl_bitmap_calc_size(mask_count)						\
({ \
	   int ___c;												\
	   ___c = mask_count / CHAR_BIT;							\
	   if(mask_count & (CHAR_BIT - 1))							\
		   ___c++;												\
	   ___c;													\
})
#endif
/** @} */

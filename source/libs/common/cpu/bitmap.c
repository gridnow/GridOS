/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   CPUλͼ����
*/
#include <limits.h>

#include "bitmap.h"
#include "bits.h"

/* Should all from hal */
#include <bitops.h>
#include <string.h> 

void cl_bitmap_init(struct cl_bitmap *cur_bitmap, unsigned long *bitmap, unsigned long mask_count)
{	
	cur_bitmap->bitmap = (unsigned long *)bitmap;	
	cur_bitmap->mask_count = mask_count;
	memset(cur_bitmap->bitmap, 0, (mask_count + (CHAR_BIT - 1)) / CHAR_BIT);
}

/**
	@brief �Զ���ĳ������������BLOCK ��λͼ
*/
bool cl_bitmap_adaption(struct cl_bitmap *bitmap, void *buffer, int buffer_size, int block_size)
{
	int total_size;
	int cur_count = 1;

	/* Ϊ�˱�֤bitmap�ķ�����unsigned long����ģ���ôblock_sizeӦ��Ҳ��unsigned long���� */
	if (block_size % sizeof(unsigned long))	
		return false;
	if ((unsigned long)buffer % sizeof(unsigned long))
		return false;

	/* Try to round the the total memory to buffer_size */
	do
	{
		total_size = cl_bitmap_calc_size(cur_count) +							// bitmap size
			cur_count * block_size;												// data part 
		
		if (total_size > buffer_size)											// ���������ˣ�
		{
			cur_count--;
			break;		
		}		
		cur_count++;															// ����һ�¸������ƽ�ҳ������
	} while (1);

	if (cur_count == 0)
		return false;

	total_size = cl_bitmap_calc_size(cur_count) + 
		cur_count * block_size;		

	/* But we put the bitmap at the end of the buffer, so the base address is aligned */
	bitmap->bitmap = buffer + cur_count * block_size;
	bitmap->mask_count = cur_count;
	memset(bitmap->bitmap, 0, cl_bitmap_calc_size(cur_count));
#if 0
	printk("Bitmap adaption count %d, buffer size %d, block size %d, addr %p, bitsize %d, �˷�%d.\n", 
		cur_count, buffer_size, block_size,
		bitmap->bitmap, cl_bitmap_calc_size(cur_count),
		buffer_size - total_size);
#endif
	return true;
}

unsigned long cl_bitmap_alloc_bit(struct cl_bitmap *cur_bitmap)
{
	unsigned long bit_offset;
	bool scan_flag = false;
again_scan:
	bit_offset = cl_find_next_zero_bit((unsigned long *)cur_bitmap->bitmap, cur_bitmap->mask_count, cur_bitmap->last_bit);
	if (bit_offset != cur_bitmap->mask_count)
	{
		__set_bit(bit_offset, (unsigned long *)cur_bitmap->bitmap);
		cur_bitmap->last_bit = bit_offset + 1;
		return bit_offset;
	}
	cur_bitmap->last_bit = 0;
	if (scan_flag == false)
	{
		scan_flag = true;
		goto again_scan;
	}
	return HAL_BITMAP_ALLOC_BIT_ERROR;
}

unsigned long cl_bitmap_alloc_consistant_bits(struct cl_bitmap *cur_bitmap, int count)
{
	int i, temp_count;
	bool scan_flag = false;
	unsigned long bit_offset;

restart_scan:
	temp_count = 0;
	bit_offset = cl_find_next_zero_bit((unsigned long *)cur_bitmap->bitmap, cur_bitmap->mask_count, cur_bitmap->last_bit);
	if (bit_offset == cur_bitmap->mask_count)
	{
		cur_bitmap->last_bit = 0;
		bit_offset = cl_find_next_zero_bit((unsigned long *)cur_bitmap->bitmap, cur_bitmap->mask_count, cur_bitmap->last_bit);
		if (bit_offset == cur_bitmap->mask_count)
		{
			goto error;
		}
	}
	cur_bitmap->last_bit = bit_offset;
	//���������õ��Ŀ���λ��λͼ���λ��ʼѰ��������count-1���Ŀ���λ
	for (i = bit_offset + 1; i < cur_bitmap->mask_count; i++, temp_count++)
	{
		if ((temp_count + 1) == count)
		{
			break;
		}
		if (test_bit(i, (unsigned long *)cur_bitmap->bitmap))
		{
			cur_bitmap->last_bit = i + 1;
			goto restart_scan;
		}
	}
	/*
		��û���ҵ�������count-1���Ŀ���λ�����ж�scan_flag��
		true�� ���λͼ�����ʼ��ַ��0λ��ʼ����Ѱ������count������λ
		false����ʧ�ܷ���
	*/
	if ((i == cur_bitmap->mask_count) && ((temp_count + 1) < count))
	{
		if (scan_flag == false)
		{
			cur_bitmap->last_bit = 0;
			scan_flag = true;
			goto restart_scan;
		}
		else
			goto error;
	}
	if ((temp_count + 1) == count)
	{		
		unsigned long first_bit = cur_bitmap->last_bit;

		for (i = 0; i < count; i++)
		{
			__set_bit(cur_bitmap->last_bit, (unsigned long *)cur_bitmap->bitmap);
			cur_bitmap->last_bit++;			
		}
		
		return first_bit;
	}
error:
	return HAL_BITMAP_ALLOC_BIT_ERROR;
}

unsigned long cl_bitmap_dealloc_consistent_bits(struct cl_bitmap *cur_bitmap, int count, unsigned long bits)
{
	int i;
	for (i = 0; i < count; i++)
	{
		if (cl_bitmap_dealloc_bit(cur_bitmap, bits++) == false)
			return i;
	}
	return count;
}

bool cl_bitmap_dealloc_bit(struct cl_bitmap *cur_bitmap, unsigned long bit)
{
	unsigned long sidx; 
	sidx = bit;
	if (!__test_and_clear_bit(sidx, (unsigned long *)cur_bitmap->bitmap))
	{
		return false;
	}

	/* 
		adjust the last_bit when dealloc bit. 
		If given bit less than the hint bit ,then change the hint bit.
	*/
	if(bit < cur_bitmap->last_bit)
		cur_bitmap->last_bit = bit;

	return true;
}


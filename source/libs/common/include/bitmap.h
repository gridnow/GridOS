/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   wuxin
*   位图操作
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
	@brief 创建bitmap对象和bitmap位图，或只创建bitmap位图

	根据cur_bitmap是否为NULL来判断是否创建bitmap对象 TRUE则创建，FALSE则不创建

	@param[in] cur_bitmap 传入的BITMAP描述体
	@param[in] bitmap 缓冲区首地址
	@param[in] mask_count  位图区域中位的个数
*/
void cl_bitmap_init(struct cl_bitmap *cur_bitmap, unsigned long *bitmap, unsigned long mask_count);

/**
	@brief 在缓冲中适应BITMAP的大小

	用户有时只知道缓冲区和每块大小，需要自动调整BITMAP尺寸

	@param[in] bitmap 传入的BITMAP描述体
	@param[in] buffer 缓冲区首地址
	@param[in] buffer_size 缓冲区尺寸
	@param[in] block_size 缓冲区平分的每块大小，如果不能被sizeof(unsigned long)整除，本函数返回false

	@return 成功返回true，失败返回false，失败主要是由于传入的参数不合适。
*/
bool cl_bitmap_adaption(struct cl_bitmap *bitmap, void *buffer, int buffer_size, int block_size);

/**
	@brief 分配一个位

	在bitmap中分配一个位后返回

	@param[in] cur_bitmap 传入的BITMAP描述体

	@return 成功返回位编号，失败则返回 HAL_BITMAP_ALLOC_BIT_ERROR
*/
unsigned long cl_bitmap_alloc_bit(struct cl_bitmap *cur_bitmap);
#define HAL_BITMAP_ALLOC_BIT_ERROR -1UL

/**
	@brief 分配连续位

	在bitmap中分配一批位，位和位之间连续，不可中断

	@param[in] cur_bitmap 传入的BITMAP描述体
	@param[in] count  要分配多少个位。

	@return 成功后返回首编号，失败则返回HAL_BITMAP_ALLOC_BIT_ERROR
*/
unsigned long cl_bitmap_alloc_consistant_bits(struct cl_bitmap *cur_bitmap, int count);

/**
	@brief 释放一个位

	在cur_bitmap中释放一个位

	@param[in] cur_bitmap 传入的BITMAP描述体
	@param[in] bit 要释放的位的编号

	@return 成功返回 true，失败返回 false
*/
bool cl_bitmap_dealloc_bit(struct cl_bitmap *cur_bitmap, unsigned long bit);

/**
	@brief 释放连续的BIT

	在bitmap中释放一批位，位和位之间连续，不可中断

	@param[in] cur_bitmap 传入的BITMAP描述体
	@param[in] count 要释放的数量
	@param[in] bit 要释放的位的起始编号
*/
unsigned long cl_bitmap_dealloc_consistent_bits(struct cl_bitmap *cur_bitmap, int count, unsigned long bits);

/**
	@brief 获取bitmap字节大小
	
	获取bitmap对象所开辟的位的字节尺寸

	@param[in] bitmap 传入的BITMAP描述提

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

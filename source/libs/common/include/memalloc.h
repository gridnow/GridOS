/**
*  @defgroup memalloc
*  @ingroup common_libs
*
*  内存分配模块
*
*  @{
*/

#ifndef COMMON_LIBS_MEMALLOC
#define COMMON_LIBS_MEMALLOC

/**
	@brief 在一个大块内存上创立分配器
	
	创立的分配器可以用于不定大小的内存块分配

	@param[in] big_block 大块起始地址，分配器所有的管理数据结构和用户分配的内存块都将存放在该块地址上
	@param[in] size 大块地址长度
	
	@return bool
*/
bool memalloc_init_allocation(void * big_block, int size);

/**
	@brief 分配一块内存

	在初始化后的一个内存块上分配指定大小的内存块

	@param[in] big_block 大内存块，必须是通过初始化接口初始化过的，否则会产生不确定后果
	@param[in] size 要分配的内存块尺寸

	@return 失败返回NULL，成功返回可用的内存块地址
*/
void * memalloc(void * big_block, int size);

/**
	@brief 释放一块内存

	在初始化后的一个内存块上释放指定的内存块

	@param[in] big_block 大内存块，必须是通过初始化接口初始化过的，否则会产生不确定后果
	@param[in] p 要被释放的内存块地址，如果该指针是NULL，直接返回NULL。如果p是一个被释放过的指针，直接返回NULL。如果是其他错误的值则会产生不确定后果

	@return 成功返回p, 失败返回NULL，或者是不确定结果
*/
void * memfree(void * big_block, void * p);

#endif
/** @} */

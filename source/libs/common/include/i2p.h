/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Jerry,Wuxin
 *   General i2p
 */


#ifndef COMMON_I2P_H
#define COMMON_I2P_H

#include <types.h>
#include <stddef.h>
#include <limits.h>

#define MAX_COUNT_PER_ARRAY			1024
#define MAX_ARRAY_SIZE				(MAX_COUNT_PER_ARRAY * sizeof(void *))
#define COMMON_I2P_ALLOC_ERROR		(-1UL)

typedef unsigned long i2p_handle;
typedef	void  (*i2p_free)(void *p);
typedef	void *(*i2p_malloc)(size_t size);
struct i2p;

/**
	@brief 创立一个i2p转换对象

	用户通过i2p转换对象实现index <-> pointer之间的双向转换操作

	@param[in] malloc_handle i2p对象中需要用到的内存分配函数
	@param[in] free_handle i2p对象中需要用到的内存回收函数

	@return 成功返回新建的i2p对象指针，失败则为 NULL;
*/
struct i2p *i2p_create(i2p_malloc malloc_handle, i2p_free free_handle);

/**
	@brief 删除i2p转换对象
	
	@param[in] i2pt 待释放的i2pt链表
*/
void i2p_delete(struct i2p *i2pt);

/**
	@brief 分配索引

	在i2pt对象中给ptr指针分配一个未用的索引

	@param[in] i2pt 待操作的i2p对象
	@param[in] ptr  传入的ptr指针 
	@return 成功返回索引，失败则为 COMMON_I2P_ALLOC_ERROR;
*/
i2p_handle i2p_alloc(struct i2p *i2pt, void *ptr);

/**
	@brief 释放i2p对象中的一个索引

	建立一个i2p对象，根据size大小，初始化i2p_node中的bitmap。

	@param[in] i2pt 传入的待操作的i2p块指针
	@param[in] index 传入的待释放的索引号
	@return 成功返回true，失败则为false;
*/
bool i2p_dealloc(struct i2p *i2pt, i2p_handle index);

/**
	@brief 通过索引，查找对应的指针。

	在i2p模块中查找对应索引项，并且使用者在查询时无需加锁，I2P 查询不会和插入删除冲突

	@param[in] i2pt 传入的待查找的i2p块指针
	@param[in] index 传入的待查找的索引号
	@return  成功返回找到的地址指针，失败则为 NULL;
*/
void *i2p_find(struct i2p *i2pt, i2p_handle index);

/**
	@brief 遍历所有句柄

	该句柄遍历所有的句柄，对遍历到的每个句柄调用action函数进行处理
	
	action函数接受2个参数process指针和handle

	@param[in] i2pt 传入的待查找的i2p链表指针
	@param[in] action 传入的处理函数指针
	@param[in] process 传递给回调函数的process指针，指示当前进程
	@return  成功遍历的句柄数;
*/
unsigned long i2p_loop(struct i2p *i2p_list, void (*action)(void *process, unsigned long handle_index), void *process);

#endif


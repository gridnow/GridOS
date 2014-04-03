
#ifndef _RING_BUFF_H
#define _RING_BUFF_H

#include <types.h>

/* 定义package报文头有效类型 */
#define INVALID_FLAG  0
#define VALID_FLAG 1
/* 还在使用中标志,禁止再次读取 */
#define BUSY_FLAG  2

/* 调用者保证多线程访问安全 */
struct ring_buff_cache
{
	/* 整个buff cache 大小 */
	size_t cache_buff_length;
	/* 可读buff cache数据的地址,该值应该是实际数据的开始处 */
	unsigned long curr_read_pos;
	
	unsigned long curr_write_pos;

	/* 是否初始化标志 */
	int is_init;
};

struct ring_package
{
	/* 该flag表示该报文是否被占用 1 占用 0 释放 2 协议栈占用*/
	int valid_flag;
	/* 下一个包头的位置,为0表示该包头为最后  */
	unsigned long next_head;
	/* 前一个包头位置,为0表示该包头为最前 */
	unsigned long prev_head;
	/* 数据位置 */
	unsigned long package_offset;
	/* 包大小 */
	size_t package_size;
};

/**
	@brief 给定一个内存区域,初始化为ring buff
	@return
		cache head 成功   NULL 失败
*/
struct ring_buff_cache * ring_buff_head_init(void *cache, size_t length);

/**
	@brief 分配一个报文头
	@return
		成功返回报文body  失败返回 NULL
*/
void *ring_buff_alloc(struct ring_buff_cache *cache, size_t length);

/**
	@brief 释放buff 并合并相邻空闲buff
	@return 
		无
*/
void ring_buff_free(struct ring_buff_cache *cache, void *buff);

/**
	@brief 获取当前可读取报文头
	@return
		有可读报文返回package  无返回 NULL
*/
struct ring_package * ring_cache_read_package(struct ring_buff_cache *cache);

/**
	@brief 调试使用,打印队列内所有的package head信息
	@return 
		无
*/
void cache_package_head_info_debug(struct ring_buff_cache *cache);

#endif






/**
	Filesystem framework
	Copyright(c) 2003-2013 Sihai(yaosihai@yeah.net). 
	All rights reserved.
*/

#ifndef FSS_CACHE_H
#define FSS_CACHE_H

#include <kernel/ke_lock.h>
#include <kernel/ke_atomic.h>
#include <kernel/ke_event.h>
#include <kernel/ke_memory.h>

#include <list.h>
#include <string.h>

#define FSS_INFINITE_WAIT_FOR_EVENT -1

/**
	@brief Cache size
*/
#define FSS_RECALL_TIME_OUT			(1000)
#define FSS_CACHE_SIZE				(8 * 1024 * 1024)

/* Recall lock */

typedef enum 
{
	DB_DIRTY		= (1 << 0),											/* 脏数据   */
	DB_WB_ERR		= (1 << 1),											/* 回写错误 */
	DB_VALID_DATA	= (1 << 2),											/* 缓冲区中的数据是有效的 */
	DB_STAIC		= (1 << 3),
} fss_cache_flags;

/**
	@brief 回收器描述体
*/
struct fss_cache_recaller
{
	struct ke_event *start_recall;										/* 回收线程等待的回收事件 */
	struct ke_event *end_recall;										/* 用户等待回收器完成的事件，所有的请求者排序阻塞在上面 */
	struct ke_atomic desired;											/* 要回回收多少块，数值上应该和等待者的数量一样，目前是一个等待者回收一块 */
};

/**
	@brief The DB Map Record (DBMR)
*/
struct dbmr
{
	struct list_head list;												/* 一个DB可能被多个进程共享，因此有列表 */
	void * process;														/* 本映射属于哪个进程 */
	void * base;														/* 本映射的基础地址 */
};

/**
	@brief The DB Descriptor (DBD)
*/
struct dbd
{
	/*
		操作锁
	*/
	struct ke_spinlock lock;
	struct list_head list;												/* LRU链表 */
	struct list_head dirty_list;										/* dirty链表 */
	/*
		数据块信息
	*/	
	unsigned long block_id;												/* 本DBD对应的文件的块 */
	struct fss_file * who;												/* 本DBD对应的文件 */

	/*
		映射信息
	*/
	void * map_lock;													/* 映射描述链表的LOCK */
	struct list_head map_list;											/* 本DB的映射描述体 */

	/*
		淘汰信息
	*/		
	struct ke_atomic access_counter;									/* 被访问了多少次 */
	unsigned long last_access_time;										/* 最近访问时间 */
	struct ke_atomic refcount;											/* 引用计数 */

	void *buffer;														/* 缓冲区*/
	ssize_t valid_size;														/* 有的文件的DB不到整块大小，这里记录有效尺寸*/
	fss_cache_flags flags;												/* 状态标志	*/										
};

/**
	@brief Cache描述体
*/
struct fss_cache
{
	unsigned long cache_size;
	unsigned int db_count;												/* DBD总块数 */

	char* db_cache;														/* DB预分配的首地址 */
	unsigned long * db_stack;											/* DB栈的首地址 */
	long db_stack_top;													/* DB栈顶 */
	struct ke_spinlock db_stack_lock;

	struct dbd *dbd_cache;												/* DBD预分配的首地址 */
	struct dbd *dbd_stack_top;											/* DB栈顶 */
	struct ke_spinlock dbd_stack_lock;

	/* LRU链表 */
	struct list_head sort_list;
	struct ke_spinlock sort_lock;
	struct fss_cache_recaller * recaller;
	
	/* Dirty list */
	struct list_head dirty_list;
	struct ke_spinlock dirty_list_lock;
};

#define FSS_DBD_LEVEL_EXCHANGE_MASK	(127)								/* Access count & this == 0时，和上一级比对是否交换位置 */
#define FSS_DBD_DEGRADE_COUNT(DBD)	((DBD)->access_counter > FSS_DBD_LEVEL_EXCHANGE_MASK ? \
	(DBD)->access_counter - FSS_DBD_LEVEL_EXCHANGE_MASK : (DBD)->access_counter = 0)

/**
	@make sure data is valid
 
 	@file descriptor and dbd pointer
 
*/

ssize_t fss_dbd_make_valid(struct fss_file * who, struct dbd * which);

/**
 	@

 	@
*/

int fss_map_init();


/**
	@brief 更新LRU排序

	@param[in] dbd :DBD指针
*/
void fss_dbd_lru_update(struct dbd * dbd);

/**
	@brief 分配新的DB空间
*/
void* fss_db_malloc(void);

/**
	@brief 分配新的DBD空间

	返回一个空闲DBD

	@param[in] who:本DBD对应的文件,用于反响映射
*/
struct dbd* fss_dbd_malloc(struct fss_file * who);

/**
	@brief 回收DBD空间

	回收DBD，从栈顶压入空闲链表，调用者保证没有人再在访问node
*/
void fss_dbd_free(struct dbd* dbd);

/**
	@brief DB cache初始化

	动态分配所需的内存，将DB与DBD相关联，并将所有DBD依次压栈，组成一个DB/DBD空闲链表
*/
bool fss_db_init(void);

/**
	@brief 释放DB cache分配的所有内存空间
*/
void fss_db_deinit(void);

/**
	@brief 线程自我主动阻塞
		
	@return 
		true on ok
		false on failure
*/
bool fss_grab_buffer(void);

/**
	@brief Init recaller, create recaller resource
*/
bool fss_cache_recaller_init();

/**
	@brief 将dbd从头部加入到lru链表中，从头部加入可以减少新数据被唤出的几率
	
	@param[in] which dbd指针
*/
void fss_dbd_lru_add(struct dbd * which);

/**
	@brief 将dbd从头部加入到dirty链表中，
	
	@param[in] which dbd指针
*/
void fss_dbd_add_to_dirty(struct dbd * which);

/************************************************************************/
/* 转换表                                                               */
/************************************************************************/
#define MLTT_PAGE_SIZE_SHIFT	(14)											/* 16KB */
#define MLTT_PAGE_SIZE			(1 << MLTT_PAGE_SIZE_SHIFT)						/* The size per table, and also used as the alignment base of the table */
#define MLTT_ENTRY_SIZE			(sizeof(void*))									/* Each entry is a pointer */
#define MLTT_ENTRY_COUNT		(MLTT_PAGE_SIZE / MLTT_ENTRY_SIZE)

#define MLTT_TABLE_POINTER_MASK (~(MLTT_PAGE_SIZE - 1))							/* Mask out the pointer of table from the entry, the table pointer is page-aligned */
#define MLTT_TABLE_COUNTER_MASK (MLTT_ENTRY_COUNT - 1)							/* Mask out the count of pointer of table from the entry */
#define MLTT_TABLE_USING		(1 << (MLTT_PAGE_SIZE_SHIFT - 1))				/* The sub table is in using and cannot be cleaned */
#define MLTT_STATIC_DBD_COUNT	16												/* 每个256kb，16个可以表示4MB的文件 */

struct fss_mltt_pos
{
	unsigned int lv1;
	unsigned int lv2;
};
struct lv1_table
{
	unsigned long ptr[MLTT_ENTRY_COUNT];
};

struct lv2_table
{
	struct lv1_table * lv1[MLTT_ENTRY_COUNT];
};

struct fss_mltt
{
	struct dbd static_dbd[MLTT_STATIC_DBD_COUNT];
	struct lv2_table *lv2;
	struct ke_spinlock lock;
};

/**
	@brief 通过访问块ID获取对应缓存描述符

	DBD 一旦获取则被锁住，防止其他线程对其修改

	@param[in] who 获取哪个文件的DBD
	@param[in] block 块号

	@return 成功返回DBD，失败返回NULL。该函数一般不会失败，除非系统内存极端耗尽。
*/
struct dbd * fss_dbd_get(struct fss_file * who, unsigned long block_id);

/**
	@brief Release the holding of a dbd

	The dbd can be manipulated by dbd-recall or other logic.

	@param[in] which the dbd to release
*/
void fss_dbd_put(struct dbd * which);

/**
	@brief Init the mltt data structure for a file

	Fill basic information for the mltt data structure of a file.

	@param[in] who the file to operate on
*/
void fss_mltt_init(struct fss_file * who);

/**
	@brief Locate the mltt entry and clean it if nobody using it

	If the whole mltt page is empty, recall it too.

	@param[in] where The dbd to be cleaned in the MLTT table

	@return
		return true if the entry has been successfully cleaned,else return false

	@note
		The caller should clean the dbd in the MLTT table before recycle the dbd to other file.
*/
bool fss_mltt_entry_clean_nolock(struct dbd * where);

/**
	@brief Make a DBD idle in the MLTT table 

	The DBD can be reused if it's in idle status

	@param[in] where The dbd to be set idle in the MLTT table
*/
//void fss_mltt_entry_idle(struct dbd * where);

/**
	@brief Aligned mem free
*/
DLLEXPORT void * fss_aligned_malloc(unsigned int size, unsigned int alignment);

/**
	@brief Release the holding of a dbd
*/
DLLEXPORT void fss_aligned_free(void * p);

/************************************************************************/
/* policy.c                                                             */
/************************************************************************/
struct dbd;
struct fss_cache_policy
{
	xstring name;
	int (*read)(struct fss_file * who, uoffset pos, struct dbd * buffer);
	int (*write)(struct fss_file * who, uoffset pos, struct dbd * buffer);
};
#define FSS_CACHE_POLICY_WB 0				/* Write Back */
#define FSS_CACHE_POLICY_NO 1				/* No cache */
#define FSS_CACHE_POLICY_WT 2				/* Write Through */
void fss_policy_init();

/**
	@brief 触发回写脏数据，阻塞当前线程

	@return 
		The count block successfully flushed
*/
int fss_cache_force_flush(int count);

/**
	@brief Get the policy descriptor

	The reader or writer need the policy methods to refill the file data

	@param[in] file the file for policy ops

	@return the policy descriptor associated with the file
*/
struct fss_cache_policy * fss_policy(struct fss_file * file);

#endif 

/** @} */

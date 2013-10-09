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
	DB_DIRTY		= (1 << 0),											/* ������   */
	DB_WB_ERR		= (1 << 1),											/* ��д���� */
	DB_VALID_DATA	= (1 << 2),											/* �������е���������Ч�� */
	DB_STAIC		= (1 << 3),
} fss_cache_flags;

/**
	@brief ������������
*/
struct fss_cache_recaller
{
	struct ke_event *start_recall;										/* �����̵߳ȴ��Ļ����¼� */
	struct ke_event *end_recall;										/* �û��ȴ���������ɵ��¼������е��������������������� */
	struct ke_atomic desired;											/* Ҫ�ػ��ն��ٿ飬��ֵ��Ӧ�ú͵ȴ��ߵ�����һ����Ŀǰ��һ���ȴ��߻���һ�� */
};

/**
	@brief The DB Map Record (DBMR)
*/
struct dbmr
{
	struct list_head list;												/* һ��DB���ܱ�������̹���������б� */
	void * process;														/* ��ӳ�������ĸ����� */
	void * base;														/* ��ӳ��Ļ�����ַ */
};

/**
	@brief The DB Descriptor (DBD)
*/
struct dbd
{
	/*
		������
	*/
	struct ke_spinlock lock;
	struct list_head list;												/* LRU���� */
	struct list_head dirty_list;										/* dirty���� */
	/*
		���ݿ���Ϣ
	*/	
	unsigned long block_id;												/* ��DBD��Ӧ���ļ��Ŀ� */
	struct fss_file * who;												/* ��DBD��Ӧ���ļ� */

	/*
		ӳ����Ϣ
	*/
	void * map_lock;													/* ӳ�����������LOCK */
	struct list_head map_list;											/* ��DB��ӳ�������� */

	/*
		��̭��Ϣ
	*/		
	struct ke_atomic access_counter;									/* �������˶��ٴ� */
	unsigned long last_access_time;										/* �������ʱ�� */
	struct ke_atomic refcount;											/* ���ü��� */

	void *buffer;														/* ������*/
	ssize_t valid_size;														/* �е��ļ���DB���������С�������¼��Ч�ߴ�*/
	fss_cache_flags flags;												/* ״̬��־	*/										
};

/**
	@brief Cache������
*/
struct fss_cache
{
	unsigned long cache_size;
	unsigned int db_count;												/* DBD�ܿ��� */

	char* db_cache;														/* DBԤ������׵�ַ */
	unsigned long * db_stack;											/* DBջ���׵�ַ */
	long db_stack_top;													/* DBջ�� */
	struct ke_spinlock db_stack_lock;

	struct dbd *dbd_cache;												/* DBDԤ������׵�ַ */
	struct dbd *dbd_stack_top;											/* DBջ�� */
	struct ke_spinlock dbd_stack_lock;

	/* LRU���� */
	struct list_head sort_list;
	struct ke_spinlock sort_lock;
	struct fss_cache_recaller * recaller;
	
	/* Dirty list */
	struct list_head dirty_list;
	struct ke_spinlock dirty_list_lock;
};

#define FSS_DBD_LEVEL_EXCHANGE_MASK	(127)								/* Access count & this == 0ʱ������һ���ȶ��Ƿ񽻻�λ�� */
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
	@brief ����LRU����

	@param[in] dbd :DBDָ��
*/
void fss_dbd_lru_update(struct dbd * dbd);

/**
	@brief �����µ�DB�ռ�
*/
void* fss_db_malloc(void);

/**
	@brief �����µ�DBD�ռ�

	����һ������DBD

	@param[in] who:��DBD��Ӧ���ļ�,���ڷ���ӳ��
*/
struct dbd* fss_dbd_malloc(struct fss_file * who);

/**
	@brief ����DBD�ռ�

	����DBD����ջ��ѹ��������������߱�֤û�������ڷ���node
*/
void fss_dbd_free(struct dbd* dbd);

/**
	@brief DB cache��ʼ��

	��̬����������ڴ棬��DB��DBD���������������DBD����ѹջ�����һ��DB/DBD��������
*/
bool fss_db_init(void);

/**
	@brief �ͷ�DB cache����������ڴ�ռ�
*/
void fss_db_deinit(void);

/**
	@brief �߳�������������
		
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
	@brief ��dbd��ͷ�����뵽lru�����У���ͷ��������Լ��������ݱ������ļ���
	
	@param[in] which dbdָ��
*/
void fss_dbd_lru_add(struct dbd * which);

/**
	@brief ��dbd��ͷ�����뵽dirty�����У�
	
	@param[in] which dbdָ��
*/
void fss_dbd_add_to_dirty(struct dbd * which);

/************************************************************************/
/* ת����                                                               */
/************************************************************************/
#define MLTT_PAGE_SIZE_SHIFT	(14)											/* 16KB */
#define MLTT_PAGE_SIZE			(1 << MLTT_PAGE_SIZE_SHIFT)						/* The size per table, and also used as the alignment base of the table */
#define MLTT_ENTRY_SIZE			(sizeof(void*))									/* Each entry is a pointer */
#define MLTT_ENTRY_COUNT		(MLTT_PAGE_SIZE / MLTT_ENTRY_SIZE)

#define MLTT_TABLE_POINTER_MASK (~(MLTT_PAGE_SIZE - 1))							/* Mask out the pointer of table from the entry, the table pointer is page-aligned */
#define MLTT_TABLE_COUNTER_MASK (MLTT_ENTRY_COUNT - 1)							/* Mask out the count of pointer of table from the entry */
#define MLTT_TABLE_USING		(1 << (MLTT_PAGE_SIZE_SHIFT - 1))				/* The sub table is in using and cannot be cleaned */
#define MLTT_STATIC_DBD_COUNT	16												/* ÿ��256kb��16�����Ա�ʾ4MB���ļ� */

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
	@brief ͨ�����ʿ�ID��ȡ��Ӧ����������

	DBD һ����ȡ����ס����ֹ�����̶߳����޸�

	@param[in] who ��ȡ�ĸ��ļ���DBD
	@param[in] block ���

	@return �ɹ�����DBD��ʧ�ܷ���NULL���ú���һ�㲻��ʧ�ܣ�����ϵͳ�ڴ漫�˺ľ���
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
	@brief ������д�����ݣ�������ǰ�߳�

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

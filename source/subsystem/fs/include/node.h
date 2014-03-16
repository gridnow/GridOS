/**
	Filesystem framework
	Copyright(c) 2003-2013 Sihai(yaosihai@yeah.net). 
	All rights reserved.
*/

#ifndef FSS_NODE_H
#define FSS_NODE_H

#include <kernel/ke_atomic.h>
#include <kernel/ke_rwlock.h>
#include <kernel/ke_critical.h>
#include <ddk/vfs.h>
#include "object.h"

#include "cache.h"
struct fss_volumn;

typedef enum
{
	FSS_FILE_TYPE_NORMAL,
	FSS_FILE_TYPE_DIR,
} fss_file_type;

struct fss_file
{
	struct cl_object obj;
	
	/* Ŀ¼������̫�ʺ��������ļ�����������TODO: ����ͨ��hash�Ľ� */
	struct list_head brother;
	struct fss_file *parent;
	struct ke_atomic ref;
	struct fss_volumn *volumn;

	/* �ļ�ϵͳ�Դ��ļ���˽������ */
	void *private;

	/* �ļ�����Ŀǰ��̬���䣬���������Ƭ(See fss_file_new)��TODO��ͨ���ַ����������Ľ� */
	char *name;
	fss_file_type type;
	/* �ļ�ͨ���� */
	struct list_head notify_list;
	/* �ļ�ͨ����ԭ�Ӳ����� */
	struct ke_spinlock notify_lock;
	
	union
	{
		struct __type_file__
		{
			/* Cache */
			struct fss_mltt mltt;

		} file;

		struct __type_dir__
		{
			struct list_head child_head;
			struct ke_critical_section dir_sleep_lock;													// Sleep lock
			struct ke_rwlock dir_lock;
			unsigned int tree_flags;
#define FSS_FILE_TREE_COMPLETION (1 << 0)											// Ŀ¼�е���Ŀ�������ģ�û�б���̭
		} dir;
	} t;
};

#define FSS_FILE_GET_NAME(FILE) ((FILE)->name)
#define FSS_FILE_INC_REF_COUNTER(FILE) (ke_atomic_inc(&(FILE)->ref))
#define FSS_FILE_GET_REF_COUNTER(FILE) (ke_atomic_read(&(FILE)->ref))

typedef enum 
{
	LOOP_FILE_END,
	LOOP_NOT_FOUND,
	LOOP_FOUND
} fss_action_reason;
typedef struct fss_file * (*fss_find_action)(struct fss_file * file, fss_action_reason reason, char* path_name, int name_left, int name_right, void *context);

/**********************************************************
 About tree 
 *********************************************************/
struct fss_file *fss_file_new(struct fss_file *father, void *private, const char *name, fss_file_type type);

/**
	@brief The main loop function of the tree
*/
struct fss_file * fss_loop_file(struct fss_file * current_path, xstring path_string, void * context, fss_find_action action);

/**
	@brief Init the tree on the volumn
*/
bool fss_tree_init(struct fss_volumn *v, struct fss_vfs_driver *drv, void *root_private);

/*
	@brief Get all file in the dirty path
*/
int fss_tree_make_full(struct fss_file * current_path);

#endif

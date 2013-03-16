/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   通用对象管理器
*/
#ifndef COMMON_OBJECT_H
#define COMMON_OBJECT_H

#include <list.h>
#include <types.h>
#include <kernel/ke_atomic.h>

#include "blkbuf.h"

struct cl_object
{
	char			*name;					/* 对象名 */
	void			*type;					/* 指向type */
	struct list_head	list;					/* 同一类对象链表 */
	struct ke_atomic	ref;					/* 对象引用计数器 */
};

struct cl_object_ops
{
	/* 进程关闭一个对象，具体子系统应该对其作出反应 */
	bool (*close)(struct cl_object *obj);

	/* 新创立的对象被初始化 */
	void (*init)(struct cl_object *object);

	/* 希望得到对象的同步对象，用于关联一些事件，互斥什么的 */
	void *(*get_sync_object)(struct cl_object *obj);	
};

enum cl_object_memory_type
{
	COMMON_OBJECT_MEMORY_TYPE_NAME,
	COMMON_OBJECT_MEMORY_TYPE_OBJ,
	COMMON_OBJECT_MEMORY_TYPE_NODE,
};

struct cl_object_type
{
	const char	*name;
	size_t		size;
	bool (*add_space)(struct cl_object_type *type, void **base, size_t *size, enum cl_object_memory_type);
	void (*free_space)(struct cl_object_type *type, void *base, size_t size, enum cl_object_memory_type);

	/* 对象操作方法 */
	struct cl_object_ops *ops;

	/* HIDE to user */
	struct cl_bkb allocator;
};

/* 方法 */
/**
	@brief 创立一个指定类型的对象	
*/
void *cl_object_create(struct cl_object_type *type);

/**
	@brief 关闭对对象的使用
*/
void cl_object_close(void *object);

/**
	@brief Register type
*/
void cl_object_type_register(struct cl_object_type *type);

#endif

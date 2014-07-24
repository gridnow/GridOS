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
#include <cl_atomic.h>
#include "blkbuf.h"

struct cl_object_type;
struct cl_object
{
	xstring					name;					/* 对象名 */
	struct cl_object_type	*type;					/* 指向type */
	struct ke_atomic		ref;					/* 对象引用计数器 */
	struct list_head		list;					/* 同一类对象链表 */
};
typedef void *real_object_t;
#define TO_CL_OBJECT(USER_OBJECT) ((struct cl_object*)(USER_OBJECT) - 1)
#define TO_USER_OBJECT(CL_OBJECT) ((real_object_t)((CL_OBJECT) + 1))
#define CL_OBJECT_NAME_SLOTE_LENGTH 64				/* 名字节点的每个slot的长度 */

struct cl_object_ops
{
	bool (*close)(void *by, real_object_t *object);
	bool (*init)(real_object_t *object);
};

enum cl_object_memory_type
{
	COMMON_OBJECT_MEMORY_TYPE_NAME,
	COMMON_OBJECT_MEMORY_TYPE_OBJ,
	COMMON_OBJECT_MEMORY_TYPE_NODE,
};

struct object_tree_node
{
	char *node_name;
	struct object_tree_node *parent;
	struct list_head son;
	struct list_head siblings;
	unsigned long flags;
};

struct cl_object_type
{
	const xstring	name;
	size_t			size;

	/* 对象缓冲区操作，用与为对象分配器创立对象内存 */
	bool (*add_space)(struct cl_object_type *type, void **base, size_t *size, enum cl_object_memory_type);
	void (*free_space)(struct cl_object_type *type, void *base, size_t size, enum cl_object_memory_type);

	/* 对象操作方法 */
	struct cl_object_ops *ops;

	/* HIDE to user */
	struct cl_bkb obj_allocator, node_allocator, name_allocator/*TODO: tree mode will not use this */;
	struct list_head unname_objects;
	struct object_tree_node *node;
};

/* Useful macro */

/* 方法 */
/**
	@brief 创立一个指定类型的对象	
*/
void *cl_object_create(struct cl_object_type *type);

/**
	@brief 关闭对对象的使用
*/
void cl_object_delete(void *object);
void cl_object_close(void *by, void *object);
void cl_object_open(void *by, void *object);
int cl_object_get_ref_counter(void *object);
void cl_object_dec_ref(void *object);
void cl_object_inc_ref(void *object);

/**
	@brief Register type
*/
void cl_object_type_register(struct cl_object_type *type);

//name.c
/**
	@brief 设置对象的名称
*/
xstring cl_object_set_name(real_object_t who, xstring what);

/**
	@brief 搜索对象，如果搜索到，则增加Ref,使用者一般情况下应该减少Ref
*/
real_object_t cl_object_search_name(struct cl_object_type *type, xstring name);

/**
	@brief 获取对象名称

	对象名称是存放在对象中，用户一般不要保存到其他地方，指针也不要间接引用。
*/
xstring cl_object_get_name(real_object_t who);

#endif

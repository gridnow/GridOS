/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   ͨ�ö��������
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
	xstring					name;					/* ������ */
	struct cl_object_type	*type;					/* ָ��type */
	struct ke_atomic		ref;					/* �������ü����� */
	struct list_head		list;					/* ͬһ��������� */
};
typedef void *real_object_t;
#define TO_CL_OBJECT(USER_OBJECT) ((struct cl_object*)(USER_OBJECT) - 1)
#define TO_USER_OBJECT(CL_OBJECT) ((real_object_t)((CL_OBJECT) + 1))
#define CL_OBJECT_NAME_SLOTE_LENGTH 64				/* ���ֽڵ��ÿ��slot�ĳ��� */

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

	/* ���󻺳�������������Ϊ������������������ڴ� */
	bool (*add_space)(struct cl_object_type *type, void **base, size_t *size, enum cl_object_memory_type);
	void (*free_space)(struct cl_object_type *type, void *base, size_t size, enum cl_object_memory_type);

	/* ����������� */
	struct cl_object_ops *ops;

	/* HIDE to user */
	struct cl_bkb obj_allocator, node_allocator, name_allocator/*TODO: tree mode will not use this */;
	struct list_head unname_objects;
	struct object_tree_node *node;
};

/* Useful macro */

/* ���� */
/**
	@brief ����һ��ָ�����͵Ķ���	
*/
void *cl_object_create(struct cl_object_type *type);

/**
	@brief �رնԶ����ʹ��
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
	@brief ���ö��������
*/
xstring cl_object_set_name(real_object_t who, xstring what);

/**
	@brief �������������������������Ref,ʹ����һ�������Ӧ�ü���Ref
*/
real_object_t cl_object_search_name(struct cl_object_type *type, xstring name);

/**
	@brief ��ȡ��������

	���������Ǵ���ڶ����У��û�һ�㲻Ҫ���浽�����ط���ָ��Ҳ��Ҫ������á�
*/
xstring cl_object_get_name(real_object_t who);

#endif

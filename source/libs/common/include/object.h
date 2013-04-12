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
#include <kernel/ke_atomic.h>
#include "blkbuf.h"

struct cl_object_type;
struct cl_object
{
	char					*name;					/* ������ */
	struct cl_object_type	*type;					/* ָ��type */
	struct ke_atomic		ref;					/* �������ü����� */
	struct list_head		list;					/* ͬһ��������� */
};
typedef void *real_object_t;

struct cl_object_ops
{
	bool (*close)(real_object_t *object);
	void (*init)(real_object_t *object);
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
	const char	*name;
	size_t		size;

	/* ���󻺳�������������Ϊ������������������ڴ� */
	bool (*add_space)(struct cl_object_type *type, void **base, size_t *size, enum cl_object_memory_type);
	void (*free_space)(struct cl_object_type *type, void *base, size_t size, enum cl_object_memory_type);

	/* ����������� */
	struct cl_object_ops *ops;

	/* HIDE to user */
	struct cl_bkb obj_allocator, node_allocator;
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
void cl_object_close(void *object);

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

#endif

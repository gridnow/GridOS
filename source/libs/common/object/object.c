/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   对象管理器
*/
#include <types.h>

#include "cl_string.h"
#include "object.h"

static void object_free_handler(void *para, void *base, size_t size)
{
	struct cl_object_type * type = para;
	type->free_space(type, base, size, COMMON_OBJECT_MEMORY_TYPE_OBJ);
}

/**
	@brief 创立一个指定类型的对象	
*/
void *cl_object_create(struct cl_object_type *type)
{	
	struct cl_object *object;
	bool expaned_memory = false;

again:
	object = (struct cl_object *)cl_bkb_alloc(&type->obj_allocator);
	if (object == NULL)
	{
		void *base;
		size_t size;
		if (expaned_memory == false)
		{
			expaned_memory = true;
			if (type->add_space(type, &base, &size, COMMON_OBJECT_MEMORY_TYPE_OBJ) == true)
			{
				/* Add the space to allocator */
				cl_bkb_extend(&type->obj_allocator, base, size, object_free_handler, type);
				goto again;
			}			
		}
		object = NULL;
		goto err;
	}
	memset(object, 0, type->size + sizeof(struct cl_object));	
	object->type = type;
	list_add_tail(&object->list, &type->unname_objects);
	
	if (type->ops->init)
	{
		if (type->ops->init((real_object_t*)(object + 1)) == false)
		{
			goto err;
		}
	}

	return object + 1;

err:
	if (object)	
		cl_object_delete(object);
	return NULL;
}

void cl_object_delete(void *object)
{
	struct cl_object *p = TO_CL_OBJECT(object);
	struct cl_object_type *type = p->type;

	list_del(&p->list);
	cl_bkb_dealloc(&type->obj_allocator, p);
}

void cl_object_open(void *by, void *object)
{
	cl_object_inc_ref(object);
//	if (p->type->ops->open)
//		p->type->ops->open(by, object);
}

int cl_object_get_ref_counter(void *object)
{
	struct cl_object *p = TO_CL_OBJECT(object);
	return p->ref.counter;
}

void cl_object_dec_ref(void *object)
{
	struct cl_object *p = TO_CL_OBJECT(object);
	cl_atomic_dec(&p->ref);
}

void cl_object_inc_ref(void *object)
{
	struct cl_object *p = TO_CL_OBJECT(object);
	cl_atomic_inc(&p->ref);
}

/**
	@brief 关闭对对象的使用
 */
void cl_object_close(void *by, void *object)
{
	struct cl_object *p = TO_CL_OBJECT(object);

	cl_object_dec_ref(object);
	if (p->type->ops->close)
		p->type->ops->close(by, object);

	/* Should be deleted? */
	if (cl_object_get_ref_counter(object) <= 0)
		cl_object_delete(object);
}

/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   对象管理器
*/

#include "object.h"
#include "cl_string.h"

static void object_free_handler(void *para, void *base, size_t size)
{
	struct cl_object_type * type = para;
	type->free_space(type, base, size, COMMON_OBJECT_MEMORY_TYPE_NAME);
}

/**
	@brief 设置对象的名称
*/
xstring cl_object_set_name(real_object_t who, xstring what)
{
	//TODO: 优化：使用名字空间来分配
	
	xstring slot;
	struct cl_object *object;
	bool expaned_memory = false;
	
	if (strlen(what) + sizeof(xchar) >= CL_OBJECT_NAME_SLOTE_LENGTH)
	{
		slot = NULL;
		goto end;
	}
	
	object = TO_CL_OBJECT(who);
	
again:
	slot = (xstring)cl_bkb_alloc(&object->type->name_allocator);
	if (slot == NULL)
	{
		void *base;
		size_t size;
		if (expaned_memory == false)
		{
			expaned_memory = true;
			if (object->type->add_space(object->type, &base, &size, COMMON_OBJECT_MEMORY_TYPE_NAME) == true)
			{
				/* Add the space to allocator */
				cl_bkb_extend(&object->type->name_allocator, base, size, object_free_handler, object->type);
				goto again;
			}
		}
		slot = NULL;
		goto end;
	}
	
	strcpy(slot, what);
	object->name = slot;
	
end:
	return slot;
}

real_object_t cl_object_search_name(struct cl_object_type *type, xstring name)
{
	struct list_head *t;
	struct cl_object *p;
	
	/* 呃，这个算法太低级了，先凑合这用着吧，TODO: HASH */
	list_for_each(t, &type->unname_objects)
	{
		p = list_entry(t, struct cl_object, list);
		if (p->name && !strcmp(p->name, name))
		{
			break;
		}
		p = NULL;
	}
	
	if (p)
	{
		real_object_t u = TO_USER_OBJECT(p);
		cl_object_inc_ref(u);
		return u;
	}
	return NULL;
}
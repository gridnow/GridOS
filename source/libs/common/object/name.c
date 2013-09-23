/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   对象管理器
*/

#include "object.h"
#include "cl_string.h"

/**
	@brief 设置对象的名称
*/
xstring cl_object_set_name(real_object_t who, xstring what)
{
	//TODO: 使用名字空间来分配
	return what;
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
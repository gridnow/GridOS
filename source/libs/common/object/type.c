/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   对象管理器
*/

#include "object.h"
#include "string.h"

/**
	@brief Register type
*/
void cl_object_type_register(struct cl_object_type *type)
{
	/* Init allocator */
	memset(&type->obj_allocator, 0, sizeof(type->obj_allocator));
	type->obj_allocator.name =(xstring)type->name;
	type->obj_allocator.node_size = type->size + sizeof(struct cl_object);
}


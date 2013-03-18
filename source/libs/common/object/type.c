/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   对象管理器
*/

#include "object.h"

/**
	@brief Register type
*/
void cl_object_type_register(struct cl_object_type *type)
{
	/* Init allocator */
	memset(&type->allocator, 0, sizeof(type->allocator));
	type->allocator.name =(xstring)type->name;
	type->allocator.node_size = type->size;
}


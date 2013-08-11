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
	cl_bkb_init(&type->obj_allocator, type->name, type->size + sizeof(struct cl_object));
	cl_bkb_init(&type->node_allocator, type->name, 0);
}


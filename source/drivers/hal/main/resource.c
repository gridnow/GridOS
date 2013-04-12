/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL Device Resource manager
*/

#include <types.h>
#include <errno.h>
#include <ddk/resource.h>
#include <ddk/log.h>
#include <ddk/slab.h>

#include <kernel/ke_lock.h>
#include <ddk/compatible.h>


static struct resource io_port_resource = {
	.name	= "PCI 输入输出端口资源",
	.start	= 0,
	.end	= IO_SPACE_LIMIT,
	.flags	= IORESOURCE_IO,
};

static struct resource io_mem_resource = {
	.name	= "PCI 内存资源",
	.start	= 0,
	.end	= -1,
	.flags	= IORESOURCE_MEM,
};
static DEFINE_SPINLOCK(resource_lock);

/* Return the conflict entry if you can't request it */
static struct resource * __request_resource(struct resource *root, struct resource *new)
{
	resource_size_t start = new->start;
	resource_size_t end = new->end;
	struct resource *tmp, **p;

	if (end < start)
		return root;
	if (start < root->start)
		return root;
	if (end > root->end)
		return root;
	p = &root->child;
	for (;;) {
		tmp = *p;
		if (!tmp || tmp->start > end) {
			new->sibling = tmp;
			*p = new;
			new->parent = root;
			return NULL;
		}
		p = &tmp->sibling;
		if (tmp->end < start)
			continue;
		return tmp;
	}
}

/*
 * Insert a resource into the resource tree. If successful, return NULL,
 * otherwise return the conflicting resource (compare to __request_resource())
 */
static struct resource * __insert_resource(struct resource *parent, struct resource *new)
{
	struct resource *first, *next;

	for (;; parent = first) {
		first = __request_resource(parent, new);
		if (!first)
			return first;

		if (first == parent)
			return first;
		if (WARN_ON(first == new))	/* duplicated insertion */
			return first;

		if ((first->start > new->start) || (first->end < new->end))
			break;
		if ((first->start == new->start) && (first->end == new->end))
			break;
	}

	for (next = first; ; next = next->sibling) {
		/* Partial overlap? Bad, and unfixable */
		if (next->start < new->start || next->end > new->end)
			return next;
		if (!next->sibling)
			break;
		if (next->sibling->start > new->end)
			break;
	}

	new->parent = parent;
	new->sibling = next->sibling;
	new->child = first;

	next->sibling = NULL;
	for (next = first; next; next = next->sibling)
		next->parent = new;

	if (parent->child == first) {
		parent->child = new;
	} else {
		next = parent->child;
		while (next->sibling != first)
			next = next->sibling;
		next->sibling = new;
	}
	return NULL;
}

/**
 * insert_resource_conflict - Inserts resource in the resource tree
 * @parent: parent of the new resource
 * @new: new resource to insert
 *
 * Returns 0 on success, conflict resource if the resource can't be inserted.
*/
#undef insert_resource_conflict /* Conflict with the ddk define */
static struct resource *insert_resource_conflict(struct resource *parent, struct resource *new)
{
	struct resource *conflict;

	spin_lock(&resource_lock);
	conflict = __insert_resource(parent, new);
	spin_unlock(&resource_lock);
	return conflict;
}

struct resource * __request_region(struct resource *parent,
	resource_size_t start, resource_size_t n,
	const char *name, int flags)
{
	struct resource *res = kzalloc(sizeof(*res), GFP_KERNEL);

	if (!res)
		return NULL;

	res->name = name;
	res->start = start;
	res->end = start + n - 1;
	res->flags = IORESOURCE_BUSY;
	res->flags |= flags;

	spin_lock(&resource_lock);

	for (;;) {
		struct resource *conflict;

		conflict = __request_resource(parent, res);
		if (!conflict)
			break;
		if (conflict != parent) {
			parent = conflict;
			if (!(conflict->flags & IORESOURCE_BUSY))
				continue;
		}

		/* 有必要再支持 */
		if (conflict->flags & flags & IORESOURCE_MUXED) {
#if 0
			add_wait_queue(&muxed_resource_wait, &wait);
			write_unlock(&resource_lock);
			set_current_state(TASK_UNINTERRUPTIBLE);
			schedule();
			remove_wait_queue(&muxed_resource_wait, &wait);
			write_lock(&resource_lock);
			continue;
#endif
			TODO("IORESOURCE_MUXED 类型的资源发生了冲突，以后再支持。\n");
		}
		/* Uhhuh, that didn't work out.. */
		kfree(res);
		res = NULL;
		break;
	}
	spin_unlock(&resource_lock);
	return res;
}

DLLEXPORT struct resource *dr_get_constance_resource(int type)
{
	if (type == DR_RESOURCE_TYPE_IO)
		return &io_port_resource;
	else if (type == DR_RESOURCE_TYPE_MEM)
		return &io_mem_resource;

	return NULL;
}

DLLEXPORT struct resource * dr_register(int type, phys_addr_t start, resource_size_t size, const char * name, int flags)
{
	struct resource * parent = NULL;

	if (type == DR_RESOURCE_TYPE_IO)
		parent = &io_port_resource;
	else if (type == DR_RESOURCE_TYPE_MEM)
		parent = &io_mem_resource;
	
	return __request_region(parent, start, size, name, flags);
}

DLLEXPORT void dr_release(int type, resource_size_t start, resource_size_t n)
{
	TODO("");
} 

DLLEXPORT struct resource * dr_insert(struct resource *parent, struct resource *new)
{
	struct resource *conflict;

	conflict = insert_resource_conflict(parent, new);
	return conflict;
}

DLLEXPORT int dr_adjust_resource(struct resource *res, resource_size_t start, resource_size_t size)
{
	struct resource *tmp, *parent = res->parent;
	resource_size_t end = start + size - 1;
	int result = -EBUSY;

	spin_lock(&resource_lock);

	if (!parent)
		goto skip;

	if ((start < parent->start) || (end > parent->end))
		goto out;

	if (res->sibling && (res->sibling->start <= end))
		goto out;

	tmp = parent->child;
	if (tmp != res) {
		while (tmp->sibling != res)
			tmp = tmp->sibling;
		if (start <= tmp->end)
			goto out;
	}

skip:
	for (tmp = res->child; tmp; tmp = tmp->sibling)
		if ((tmp->start < start) || (tmp->end > end))
			goto out;

	res->start = start;
	res->end = end;
	result = 0;

out:
	spin_unlock(&resource_lock);
	return result;
}
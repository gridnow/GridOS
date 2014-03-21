/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   内核 针对用户层的接口句柄
 */

#include <kernel/ke_memory.h>

#include <ddk/debug.h>

#include "i2p.h"
#include "object.h"

#include <thread.h>
#include <process.h>
#include <handle.h>

static unsigned long allocated_size;

static void * get_handle_table(struct ko_process * on)
{
	spin_lock(&on->handle_lock);
	return on->handle_table;
}

static void put_handle_table(struct ko_process * on)
{
	spin_unlock(&on->handle_lock);
}

static void * handle_space_alloc(size_t size)
{
	void * p = km_valloc(size);
	allocated_size += size;
	return p;
}

static void handle_space_free(void * p)
{
	km_vfree(p);
}

ke_handle ke_handle_create(void *kobject)
{
	ke_handle h;
	void * handle_table;
	struct ko_process *on = KP_CURRENT();
	
	handle_table = get_handle_table(on);
	
	/* Sanity */
	if (handle_table == NULL)
		return KE_INVALID_HANDLE;
	
	h = i2p_alloc(handle_table, kobject);
	if (h == COMMON_I2P_ALLOC_ERROR)
	{
		h = KE_INVALID_HANDLE;
		goto end;
	}
	cl_object_inc_ref(kobject);
end:
	put_handle_table(on);
	
	return h;
}

bool ke_handle_delete(ke_handle handle)
{
	void *handle_table;
	struct ko_process *on = KP_CURRENT();
	bool r;
	
	handle_table = get_handle_table(on);
	
	r = i2p_dealloc(handle_table, handle);
	
	put_handle_table(on);
	
	return r;
}

void *ke_handle_translate(ke_handle handle)
{
	void *kobject;
	void *handle_table;
	struct ko_process * on = KP_CURRENT();
	
	handle_table = get_handle_table(on);
	
	kobject = i2p_find(handle_table, handle);

	/* We have to inc ref, because other peaple may put it to zero. */
	if (kobject)
		cl_object_inc_ref(kobject);
	
	put_handle_table(on);
	
	return kobject;
}

void ke_handle_put(ke_handle handle, void *kobject)
{
	cl_object_dec_ref(kobject);
}

ke_handle ke_handle_create_on_specific(struct ko_process *on, void *kobject)
{
	ke_handle h;
	void * handle_table;
	
	handle_table = get_handle_table(on);
	
	h = i2p_alloc(handle_table, kobject);
	
	put_handle_table(on);
	
	return h;
}

void *ke_handle_translate_no_lock(struct ko_process *on, ke_handle handle)
{
	void *handle_table;
	
	handle_table = on->handle_table;
	
	/* Find the object by handle */
	return i2p_find(handle_table, handle);
}

unsigned long ke_handle_loop(struct ko_process *on, void (*action)(struct ko_process * on, ke_handle handle))
{
	void * handle_table;
	unsigned long count = 0;
	
	handle_table = get_handle_table(on);
	
	/* May be a new process */
	if (!handle_table) goto end;
	
	/* Loop the object by handle */
	count = i2p_loop(handle_table, (void (*)(void *, ke_handle))action, on);
	
end:
	put_handle_table(on);
	return count;
}

bool ke_handle_init(struct ko_process *on)
{
	on->handle_table = i2p_create(handle_space_alloc, handle_space_free);
	if (!on->handle_table)
		return false;
	return true;
}

void ke_handle_deinit(struct ko_process *on)
{
	if (on->handle_table)
	{
		i2p_delete(on->handle_table);
		on->handle_table = NULL;
	}
}
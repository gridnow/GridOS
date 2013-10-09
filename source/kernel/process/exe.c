/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   进程可执行对象
*/
#include <kernel/kernel.h>
#include <ddk/debug.h>

#include <memory.h>
#include <process.h>
#include <section.h>
#include <exe.h>
#include <page.h>

#include "string.h"
#include "object.h"
static struct ko_process *init_process, *kernel_file_process;

static bool object_close(real_object_t *obj)
{

}

static void object_init(real_object_t *obj)
{

}

static struct cl_object_ops object_ops = {
	.close				= object_close,
	.init				= object_init,
};

static bool alloc_space(struct cl_object_type *type, void **base, size_t *size, enum cl_object_memory_type memory_type)
{
	void *object_memory;
	int count = 0;

	switch (memory_type)
	{
	case COMMON_OBJECT_MEMORY_TYPE_NAME:
	case COMMON_OBJECT_MEMORY_TYPE_OBJ:
	case COMMON_OBJECT_MEMORY_TYPE_NODE:
		count = 1;
		break;
	}
	if (count == 0) goto err;

	object_memory = km_page_alloc_kerneled(count);
	if (!object_memory) goto err;
	*base = object_memory;
	*size = count * PAGE_SIZE;
	return true;

err:
	return false;
};

static void free_space(struct cl_object_type *type, void *base, size_t size, enum cl_object_memory_type memory_type)
{

};

static struct cl_object_type exe_type = {
	.name		= "可执行对象",
	.size		= sizeof(struct ko_exe),
	.ops		= &object_ops,
	.add_space	= alloc_space,
	.free_space	= free_space,
};

/**
	@brief 创立一个空的进
*/
struct ko_exe *kp_exe_create(struct ko_section *backend, void *ctx)
{
	struct ko_exe *p;

	p = cl_object_create(&exe_type);
	if (!p) goto err;
	
	cl_object_inc_ref(backend);
	p->backend = backend;
	memcpy(KO_EXE_TO_PRIVATE(p), ctx, kp_exe_get_context_size());
	
	return p;
	
err1:
	cl_object_close(p);
err:
	return NULL;
}

struct ko_exe *kp_exe_create_temp()
{
	struct ko_section ts;
	struct ko_exe te;
	return kp_exe_create(&ts, KO_EXE_TO_PRIVATE(&te));
}

#include <elf2/elf.h>
bool kp_exe_bind(struct ko_process *who, struct ko_exe *what)
{
	int i;
	struct ko_section *ks, *sub;
	unsigned long base, size;
	
	base = elf_get_mapping_base(KO_EXE_TO_PRIVATE(what), &size, NULL);
	ks = ks_create(who, KS_TYPE_EXE, base, size, KM_PROT_READ);
	if (!ks)
	{
		TODO("");
		goto err;
	}
	cl_object_inc_ref(what);
	ks->priv.exe.exe_object = what;
	
	for (i = 0; ; i++)
	{
		struct elf_segment seg;

		if (elf_read_segment(KO_EXE_TO_PRIVATE(what), &seg, i) == false)
			break;
		sub = ks_sub_create(who, ks, seg.vstart, seg.vsize);
		if (!sub)
			goto err;

		/* Some useful matters */
		if (seg.flags & ELF_SEG_WRITE)
			sub->prot |= KM_PROT_WRITE;
		sub->priv.share.size = seg.fsize;
		sub->priv.share.offset = seg.foffset;
		//printk("sub is = %x, start %x, size = %x.\n", sub, seg.log_start, seg.size_in_log);
	}
	if (i == 0)
		goto err;

	return true;
err:
	if (ks)
	{
		TODO("删除未用对象");
	}

	return false;
}

bool kp_exe_share(struct ko_process *where, struct ko_section *ks_dst, unsigned long to, struct ko_exe *ke_src)
{
	int ret;
	unsigned long source;
	struct km *dst_mem, *src_mem;

	source = ke_src->backend->node.start + ks_dst->priv.share.offset;	/* Base + section offset*/
	source += to - ks_dst->node.start;									// Offset of the to Current process

share_again:
	dst_mem = kp_get_mem(where);
	src_mem = kp_get_mem(kp_get_file_process());
	ret = km_page_share(dst_mem, to, src_mem, source, KM_PROT_READ);
	kp_put_mem(dst_mem);
	kp_put_mem(src_mem);

	if (ret != KM_PAGE_SHARE_RESULT_OK)
	{
		if (ret == KM_PAGE_SHARE_RESULT_ERR)
			goto err;
		else if (ret == KM_PAGE_SHARE_RESULT_SRC_INVALID)
		{
			if (ks_restore(kp_get_file_process(), ke_src->backend, source) == false)
				goto err;
			goto share_again;
		}
	}

	return true;
	
err:
	return false;
}

int kp_exe_get_context_size()
{
	return elf_get_private_size();
}

bool kp_exe_copy_private(struct ko_exe *ke, void *dst_ctx, int dst_size)
{
	if (dst_size < kp_exe_get_context_size())
		return false;
	memcpy(dst_ctx, KO_EXE_TO_PRIVATE(ke), kp_exe_get_context_size());
	return true;
}

struct ko_exe *kp_exe_create_from_file(xstring name, struct ko_section *ks, void *ctx)
{
	struct ko_exe *p;
	
	p = kp_exe_create(ks, ctx);
	if (!p)
		goto err;
	if (cl_object_set_name(p, name) == NULL)
		goto err;
	
	return p;
	
err:
	if (p)
	{
		TODO_ROLL_BACK();
	}
	return NULL;
}

struct ko_exe *kp_exe_open_by_name(struct ko_process *who, xstring name)
{
	struct ko_exe *ke;

	/* Search will get reference count */
	ke = cl_object_search_name(&exe_type, name);
	if (!ke)
		goto err;

	if (kp_exe_bind(who, ke) == false)
		goto err;

	return ke;

err:
	if (ke)
		cl_object_close(ke);
	
	return NULL;
}

void kp_exe_init()
{
	exe_type.size += elf_get_private_size();
	cl_object_type_register(&exe_type);
}

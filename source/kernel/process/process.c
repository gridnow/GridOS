/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   进程管理
*/

#include <process.h>
#include <memory.h>
#include <exe.h>

#include "object.h"
static struct ko_process *init_process, *kernel_file_process;

static bool object_close(real_object_t *obj)
{

}

static void object_init(real_object_t *obj)
{
	struct ko_process *p = (struct ko_process *)obj;
	INIT_LIST_HEAD(&p->vm_list);
	ke_spin_init(&p->vm_list_lock);
	
	/* 
		Although init_process will call it, but it dose not matter for a page disappeared
		created in walk as root table.
	 */
	km_walk_init(&p->mem_ctx);
}

static struct cl_object_ops process_object_ops = {
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

static struct cl_object_type process_type = {
	.name		= "进程对象",
	.size		= sizeof(struct ko_process),
	.ops		= &process_object_ops,
	.add_space	= alloc_space,
	.free_space	= free_space,
};

/**
	@brief 获取系统进程对象
*/
struct ko_process *kp_get_system()
{
	return init_process;
}

/**
	@brief 获取文件进程
*/
struct ko_process *kp_get_file_process()
{
	return kernel_file_process;
}

/**
	@brief 创立一个空的进程对象
*/
struct ko_process *kp_create(int cpl, xstring name)
{
	struct ko_process *p;
	
	if (cpl != KP_CPL0 && cpl != KP_USER)
		goto err;
	p = cl_object_create(&process_type);
	if (!p) goto err;
	
	p->cpl = cpl;
	if (!cl_object_set_name(p, name))
		goto err1;
	
	return p;
	
err1:
	cl_object_close(p);
err:
	return NULL;
}

bool kp_init()
{
	cl_object_type_register(&process_type);

	/* Core process */
	init_process = kp_create(KP_CPL0, "操作系统核心进程");
	if (!init_process) 
		goto err;
	km_walk_init_for_kernel(&init_process->mem_ctx);

	/* Kernel file process */
	kernel_file_process = kp_create(KP_CPL0, "操作系统文件进程");
	
	kp_exe_init();
	return true;
err:
	return false;
}

/************************************************************************/
/* Interface                                                            */
/************************************************************************/

#include <sys/elf.h>
#include <ddk/ddk_for_linux.h>
#include <kernel/ke_memory.h>

#include <thread.h>
#include <section.h>

#include "misc.h"

/**
	@brief Map driver framework to kernel
*/
void ke_startup_driver_process(void *physical_data, size_t size)
{
	unsigned long entry_address;
	unsigned long base;
	Elf32_Ehdr *hdr = (Elf32_Ehdr*)physical_data;

	if (hdr->e_ident[EI_CLASS] == ELFCLASS32)
	{
		entry_address = hdr->e_entry;
	}
	else if (hdr->e_ident[EI_CLASS] == ELFCLASS64)
	{
		Elf64_Ehdr *hdr64 = (Elf64_Ehdr*)physical_data;
		entry_address = hdr64->e_entry;
	}

	/* 64kb offset is not base */
	base = entry_address & (~0xffff);
	// TODO: size of the map should contain bss, and notify system that bss should not be allocated for normal use */
	
	/* Map physical to base */
	if (km_map_physical(HAL_GET_BASIC_PHYADDRESS(physical_data), size,
					base | KM_MAP_PHYSICAL_FLAG_WITH_VIRTUAL | KM_MAP_PHYSICAL_FLAG_NORMAL_CACHE) == NULL)
		goto err;

	((void(*)())entry_address)(&ddk);
	return;
err:
	printk("驱动包影射到内核空间失败，可能是地址冲突，要影射的地址范围为%d@%x.\n", size, base);
}

/**
	@brief run startup process
 */
void ke_run_first_user_process(void *data, int size)
{
	void *entry_address;
	struct ko_section	*ks;
	struct ko_process	*kp;
	struct ko_exe		*ke;

	struct kt_thread_creating_context ctx = {0};

	printk("启动用户第一个可执行文件%x, size %d.\n", data, size);

	if (size == 0 || size == 1)
		goto err;
	/* Create file mapping */
	ks = ks_create(kp_get_file_process(), KS_TYPE_FILE, 0, size, KM_PROT_READ);
	if (!ks)
		goto err;
	
	/* Analyze the file */
	entry_address = 0;//TODO

	/* Create process for it */ 
	kp = kp_create(KP_USER, "初始化进程");
	ke = kp_exe_create(ks);
	kp_exe_bind(kp, ke);

	/* And the first thread */
	ctx.thread_entry	= entry_address;
	ctx.fate_entry		= entry_address/* special first thread */;
	ctx.flags			= KT_CREATE_RUN;
	if (kt_create(kp, &ctx) == NULL)
		goto err;

	return;
err:
	printk("启动用户的第一个进程失败。\n");
}

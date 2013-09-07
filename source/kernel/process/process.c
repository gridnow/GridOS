/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   ���̹���
*/

#include <process.h>
#include <memory.h>

#include "object.h"
static struct ko_process *init_process;

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
	.name		= "���̶���",
	.size		= sizeof(struct ko_process),
	.ops		= &process_object_ops,
	.add_space	= alloc_space,
	.free_space	= free_space,
};

/**
	@brief ��ȡϵͳ���̶���
*/
struct ko_process *kp_get_system()
{
	return init_process;
}

/**
	@brief ����һ���յĽ��̶���
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
	init_process = kp_create(KP_CPL0, "OS");
	if (!init_process) 
		goto err;
	km_walk_init_for_kernel(&init_process->mem_ctx);
	
	return true;
err:
	return false;
}

#include <sys/elf.h>
#include <ddk/ddk_for_linux.h>
#include <kernel/ke_memory.h>

/**
	@brief Map driver framework to kernel
*/
void kp_startup_driver_process(void *physical_data, size_t size)
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
	printk("������Ӱ�䵽�ں˿ռ�ʧ�ܣ������ǵ�ַ��ͻ��ҪӰ��ĵ�ַ��ΧΪ%d@%x.\n", size, base);
}

/**
	@brief Map driver framework to kernel
 */
void kp_run_user()
{
	
}
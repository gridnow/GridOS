/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   ���̹���
*/

#include <process.h>
#include <memory.h>
#include <thread.h>
#include <exe.h>
#include <section.h>
#include <handle.h>

#include "string.h"
#include "object.h"
static struct ko_process *init_process, *kernel_file_process;
struct file_process_startup_para
{
	void *data;
	int size;
	unsigned long ks_start;
};

static void file_thread(void *para)
{
	struct file_process_startup_para *package = para;
	printk("File thread startup, startup file %x, size %d, ks addr %x...", package->data, package->size, package->ks_start);
	
	memcpy((void*)package->ks_start, package->data, package->size);
	package->data = NULL;
	printk("Copy ok.\n");
	while(1)
	{
		kt_schedule();
	}
	
}

static bool object_close(void *by, real_object_t *obj)
{
	return true;
}

static bool object_init(real_object_t *obj)
{
	struct ko_process *p = (struct ko_process *)obj;
	
	/* 
		Although init_process will call it, but it dose not matter for a page disappeared
		created in walk as root table.
	 */
	km_walk_init(&p->mem_ctx);
	km_arch_ctx_init(&p->mem_ctx);

	return true;

err:
	return false;
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
	@brief ��ȡ�ļ�����
*/
struct ko_process *kp_get_file_process()
{
	return kernel_file_process;
}

/**
	@brief ����һ���յĽ��̶���
*/
struct ko_process *kp_create(int cpl, xstring name)
{
	struct ko_process *p;
	
	if (cpl != KP_CPL0 && cpl != KP_CPL0_FAKE && cpl != KP_USER)
		goto err;
	p = cl_object_create(&process_type);
	if (!p) goto err;
	
	p->cpl = cpl;
	if (!cl_object_set_name(p, name))
		goto err1;
	
	if (ks_init_for_process(p) == false)
		goto err;
	
	return p;
	
err1:
	cl_object_close(NULL, p);
err:
	return NULL;
}

bool kp_init()
{	
	cl_object_type_register(&process_type);
	
	/* Core process */
	init_process = kp_create(KP_CPL0, "����ϵͳ���Ľ���");
	if (!init_process) 
		goto err;
	km_walk_init_for_kernel(&init_process->mem_ctx);

	kt_init();
		
	kp_exe_init();
	ke_srv_init();
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
	if (km_map_physical(HAL_GET_BASIC_PHYADDRESS(physical_data), size + 0x60000/*BSS?*/,
					base | KM_MAP_PHYSICAL_FLAG_WITH_VIRTUAL | KM_MAP_PHYSICAL_FLAG_NORMAL_CACHE) == NULL)
		goto err;

	 ((void(*)())entry_address)(&ddk);
	return;
err:
	printk("������Ӱ�䵽�ں˿ռ�ʧ�ܣ������ǵ�ַ��ͻ��ҪӰ��ĵ�ַ��ΧΪ%d@%x.\n", size, base);
}

struct ko_process *kp_run_user(struct ko_exe *ke, char *cmdline)
{
	struct ko_process *kp = NULL;
	struct kt_thread_creating_context ctx = {0};

	/* Create process for it */
	kp = kp_create(KP_USER, "��ʼ������");
	if (!kp)
		goto err;
	if (kp_exe_bind(kp, ke) == NULL)
		goto err;
	
	ke_handle_init(kp);
	
	/* And the first thread (entry is absolute address) */
	ctx.thread_entry	= ke->entry;
	ctx.fate_entry		= ke->entry/* special first thread */;
	ctx.flags			= KT_CREATE_RUN | KT_CREATE_STACK_AS_PARA;
	ctx.para			= (unsigned long)cmdline;
	if (kt_create(kp, &ctx) == NULL)
		goto err;

	return kp;
err:
	if (ke)
	{
		//TODO free it
	}
	if (kp)
	{
		//TODO free it
	}
	return NULL;
}

/**
	@brief run startup process
 */
void ke_run_first_user_process(void *data, int size, char *cmdline)
{
	char *first_space, first_exe_name[128] = {0};
	struct ko_section *ks;
	void *entry_address;
	struct ko_exe *kee, *ke_tmp = kp_exe_create_temp();

	first_space = strchr(cmdline, ' ');
	if (!first_space)
		goto err;
	if (first_space - cmdline >= sizeof(first_exe_name) - sizeof(xchar))
		goto err;
	strncpy(first_exe_name, cmdline, first_space - cmdline);

	/* Kernel file process, because early we cannot copy lv2 table of driver */
	kernel_file_process = kp_create(KP_CPL0_FAKE, "����ϵͳ�ļ�����");
	if (!kernel_file_process)
		goto err;
	printk("�����û���һ����ִ���ļ�%x, size %d.\n", data, size);
	
	if (size == 0/*no file*/ || size == 1/*buffer error*/)
		goto err;
	if (elf_analyze(data, size, &entry_address, KO_EXE_TO_PRIVATE(ke_tmp)) == false)
		goto err;

	ks = ks_create(kp_get_file_process(), KS_TYPE_PRIVATE, 0, size, KM_PROT_READ|KM_PROT_WRITE);
	if (!ks)
		goto err;
	kee = kp_exe_create_from_file(first_exe_name, ks, KO_EXE_TO_PRIVATE(ke_tmp), entry_address);		
	if (!kee)
		goto err;
	
	/* But this is a special one, we have to fill the data on it */
	do {
		
		struct kt_thread_creating_context ctx = {0};
		
		struct file_process_startup_para package;
		
		package.ks_start	= ks->node.start;
		package.size		= size;
		package.data		= data;
		ctx.thread_entry	= file_thread;
		ctx.flags			= KT_CREATE_RUN;
		ctx.para			= (unsigned long)&package;
		if (kt_create(kernel_file_process, &ctx) == NULL)
			goto err;
		
		/* Wait it to finish */
		while (package.data)
			kt_schedule();
	} while(0);
	
	/* OK, create the process */
	if (kp_run_user(kee, ++first_space/*Real application*/) == false)
		goto err;
	
	return;
err:
	printk("������һ��ִ���ļ�ʧ�ܡ�\n");

}
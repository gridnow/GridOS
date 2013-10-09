/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *
 *   ��̬������
 */

#include <types.h>
#include <list.h>
#include <errno.h>
#include <string.h>
#include <compiler.h>

#include <kernel/ke_srv.h>

#include "sys/ke_req.h"

#include "../../libs/elf2/elf.h"

/************************************************************************/
/* Note:                                                                */
/* Must not in BSS, ld.sys not handled BSS by kernel                    */
/************************************************************************/

/* ���Ա����ڴ�����ִ���ļ�����������������ֱ���������Ǽ������ */
static struct list_head objs_linear_list = {(void*)1};

/* ���������ڴ��������ֱ������ */
struct dependency_list
{
	struct exe_objects * obj;
	struct dependency_list * next;
};

struct elf_context
{
	char data[512];
};

struct exe_objects 
{
	xstring name;
	struct list_head linear_list;
	struct elf_context exe_desc;
	struct elf_user_ctx user_ctx;
	unsigned long handle;
	struct dependency_list* d_list;
};
#define DBG_PREFIX "DLװ������"
#define ELF_MAX_STATIC_DL_IMAGE_INFO 512
static struct exe_objects static_elf[ELF_MAX_STATIC_DL_IMAGE_INFO] = {(void*)1};
static char static_usage[ELF_MAX_STATIC_DL_IMAGE_INFO] = {1};
static struct dependency_list static_d_list[ELF_MAX_STATIC_DL_IMAGE_INFO] = {(void*)1};	/* dependency list */
static char static_d_list_usage[ELF_MAX_STATIC_DL_IMAGE_INFO] = {1};

/* �����д��� */
static xchar cmdline_buffer[SYSREQ_PROCESS_STARTUP_MAX_SIZE] = {' '};

static void *_map_exe_file(xstring name, int *size)
{
	void *base;
	struct sysreq_process_ld req = {0};

	req.base.req_id		= SYS_REQ_KERNEL_PROCESS_HANDLE_EXE;
	req.name			= name;
	req.function_type	= SYSREQ_PROCESS_MAP_EXE_FILE;
	base = (void*)system_call(&req);
	
	*size = req.context_length;
	return base;
}

static void _unmap_exe_file(void *base)
{
	struct sysreq_process_ld req = {0};
	
	req.base.req_id		= SYS_REQ_KERNEL_PROCESS_HANDLE_EXE;
	req.name			= base;
	req.function_type	= SYSREQ_PROCESS_UNMAP_EXE_FILE;
	system_call(&req);
}

static bool _inject_exe_object(xstring name, void *ctx, int ctx_size)
{
	struct sysreq_process_ld req = {0};
	
	req.base.req_id		= SYS_REQ_KERNEL_PROCESS_HANDLE_EXE;
	req.name			= name;
	req.context			= ctx;
	req.context_length	= ctx_size;
	req.function_type	= SYSREQ_PROCESS_ENJECT_EXE;
	return system_call(&req);
}

/**
	@brief print the string without libc
*/
void early_print(char * string)
{
	struct sysreq_process_printf req = {0};
	req.base.req_id = SYS_REQ_KERNEL_PRINTF;
	req.string = string;
	system_call(&req);
}

static int h2c(char *p,unsigned long hex)
{	
	int i;
	int j=((sizeof(void*))*8)/4;						//*8 means bits,/4 means a char represent 4 bits
	unsigned long mask,old=hex;

	j--;												//j-- means goto the end of the string
	for(i=j;i>=0;i--)
	{
		hex&=0xf;
		if(hex<=9)
			hex+='0';
		else
			hex='a'+(hex-0xa);
		*(p+i)=(char)hex;
		old>>=4;
		hex=old;
	}
	j++;												//goto the hex string end
	*(p+j)='h';
	j++;												//point to the next usable position
	return j;
}

/**
	@brief get exe_objects from linear_list
*/
static struct exe_objects * get_obj_from_linear_list(char * name)
{
	struct exe_objects * obj;
	struct list_head * node;

	/* search linear list  */
	list_for_each(node, &objs_linear_list)
	{
		obj = list_entry(node, struct exe_objects, linear_list);

		/* �鿴�õ���exe_objects�����Ƿ�����Ҫ�� */
		if (!strcmp(obj->name, name))
			break;
		else
			obj = NULL;
	}

	return obj;
}

/**
	@brief Allocate a private context for an image to store the basic elf info
*/
static struct exe_objects * _allocate_static_exe()
{
	int i = 0;

	/* Loop the array to get a free one */
	for (i = 0; i < ELF_MAX_STATIC_DL_IMAGE_INFO; i++)
	{
		if (static_usage[i] == 0)
		{
			static_usage[i] = 1;
			return &static_elf[i];
		}
	}

	return NULL;
}

/**
	@brief Allocate a private dependency_list
*/
static struct dependency_list * _allocate_static_d_list()
{
	int i = 0;

	/* Loop the array to get a free one */
	for (i = 0; i < ELF_MAX_STATIC_DL_IMAGE_INFO; i++)
	{
		if (static_d_list_usage[i] == 0)
		{
			static_d_list_usage[i] = 1;
			return &static_d_list[i];
		}
	}

	return NULL;
}


/**
	@brief exe_objects insert to dependency list
*/
static void add_obj_to_dependency_list(struct exe_objects * dependency, struct exe_objects * obj)
{
	struct dependency_list * node = _allocate_static_d_list();
	node->obj = dependency;

	/* ��image������������������������� */
	node->next			= obj->d_list->next;
	obj->d_list->next	= node;
}

/**
	@brief �����ִ���ļ�������װ�ڹ�����ֱ������ʷ�ģ�������װ��
 
	@note
		name �������ļ��ľ���·��
*/
static ke_handle load(xstring name, struct exe_objects * obj)
{
	struct sysreq_process_ld req = {0};
	ke_handle image_handle;
	int new_loaded = 0;
	
open_again:
	req.base.req_id		= SYS_REQ_KERNEL_PROCESS_HANDLE_EXE;
	req.name			= name;
	req.context			= &obj->exe_desc;
	req.context_length	= elf_get_private_size();
	req.function_type	= SYSREQ_PROCESS_OPEN_EXE;
	image_handle = system_call(&req);
	obj->user_ctx.base = req.map_base;

	if (image_handle != KE_INVALID_HANDLE)
	{
		//early_print("Exe object open success.\n");
	}
	else if (new_loaded == 0)
	{
		int file_size;
		void *entry_address;
		void *file;

		file = _map_exe_file(name, &file_size);
		if (!file)
			goto err;
		if (elf_analyze(file, file_size, &entry_address, &obj->exe_desc) == false)
			goto end1;
		if (_inject_exe_object(name, &obj->exe_desc, elf_get_private_size()) == false)
			goto end1;
		
		new_loaded = 1;
		
end1:
		_unmap_exe_file(file);
		if (new_loaded == 1)
			goto open_again;
		goto err;
	}
	
	return image_handle;
	
err:
	return KE_INVALID_HANDLE;
}

static struct exe_objects *load_image(xstring name)
{
	struct exe_objects *p;

	p = _allocate_static_exe();
	if (!p) goto err; 
	//TODO allocate dynamic exe

	/* ���������û�о���·�����ϳɾ���·��������ײ������ƥ����� */
	//TODO

	/* Load from disk */
	p->handle = load(name, p);
	if (p->handle == KE_INVALID_HANDLE) goto err;

	/* �����ʧ�ܣ����Գ��Զ�̬���б��е�·��ȥ�򿪿�ִ���ļ� */
	//TODO

	p->name = name;

	/* ����������������Ա�linear list�� */
	list_add_tail(&p->linear_list, &objs_linear_list);

	return p;
err:
	if (p)
	{
		//TODO: DELETE P;
	}
	return NULL;
}

/**
	@brief load a dependent image 

	@return
		0 : end;
		1 : OK;
		-1: Error;
*/
static int load_dependency(struct exe_objects *p, int id)
{
	struct exe_objects *obj;
	xstring name;

	/* Get the name of this id */
	name = elf_get_needed(&p->exe_desc, id, &p->user_ctx);
	if (!name) return 0;

	/* ��������Ѵ�����linear list�У���û��Ҫ��ִ��load_image��ȡ */
	obj = get_obj_from_linear_list(name);
	if (!obj)
	{
		obj = load_image(name);
		if (!obj) return -1;
	}

	/* ������������Ϊ�䴴��dependency list.TODO: alloc dynamic node. */	
	if (NULL == p->d_list)
		p->d_list = _allocate_static_d_list();	
	if (NULL == p->d_list)
		return -1;

	/* ��p�����Ķ������p��dependency list�� */
	add_obj_to_dependency_list(obj, p);
	return 1;
}

/**
	@brief load all dependent images for image 
*/
static int load_dependencies()
{
	int id, ret = 0;
	struct exe_objects * obj;
	struct list_head * node;

	/* ��һ��ѭ��ʱ��node�����first image */
	list_for_each(node, &objs_linear_list)
	{	
		/* ��linear list��ȡ��image��linear listͷ������first image */
		obj = list_entry(node, struct exe_objects , linear_list);
 		//early_print("Loading dep for ");early_print(obj->name);early_print(".\n");

		/* ����image������ */
		for (id = 0; ; id++)
		{
			ret = load_dependency(obj, id);

			/* Error */
			if (ret < 0)
			{
#if 1
				char * dep = elf_get_needed(&obj->exe_desc, id, &obj->user_ctx);
				early_print("Loading dep ");
				early_print(dep);

				early_print(" ʧ�ܣ����Ǽ���.\n");
				//return ret;
#endif
			}
			/* End */
			else if (ret == 0)
			{
				break;
			}
		}
	}

	return ret;
}

extern void so_lazy_link();
static unsigned long undefined_function()
{
	return -ENOSYS;
}

/**
	@brief do lazy linking
*/
unsigned long lazy_get_symbole_by_id(unsigned long mode_base, int relocate_id)
{
// 	char str[32];
	struct elf_context *p = (struct elf_context *)mode_base;
	struct exe_objects *object = container_of(p, struct exe_objects, exe_desc);

// 	early_print("lazy_get_symbole_by_id, base: ");
// 	str[elf2_h2c(str, mode_base)] = 0;
// 	early_print(str);
// 	early_print(" ID is ");
// 	str[elf2_h2c(str, relocate_id)] = 0;
// 	early_print(str);
// 	early_print("\n");

	/**
		relocation_array[relocate_id] to relocate.
	*/
	elf_relocate_by_id(p, relocate_id, &object->user_ctx);
	return (unsigned long)undefined_function;
}

static void handle_bss(struct exe_objects *exe)
{
	int i;
	struct elf_segment seg;
	volatile unsigned char *last_byte;

	for (i = 0; ; i++)
	{
		if (elf_read_segment(&exe->exe_desc, &seg, i) == false)
			break;
		if (seg.fsize == seg.vsize)
			continue;

		/* Get the address of the last byte of valid file mapping */
		last_byte = (unsigned char*)(seg.fsize - 1 + seg.vstart);

		/* Reading the last byte cause exe refill exception of sharing, writing cause cow */
		*last_byte = *last_byte;

		/* Set the space after valid_size to zero to clean the "topmost" bss part */
		memset((void*)last_byte + 1, 0, seg.align - ((unsigned long)(last_byte + 1) & (seg.align - 1)));
	}
}

/**
	@brief relocate image
*/
static int relocation()
{
	int id, ret;
	struct exe_objects *obj;
	struct list_head * node;
	
	/* ��һ��ѭ��ʱ��node�����first image */
	list_for_each(node, &objs_linear_list)
	{	
		/* ��linear list��ȡ��image��linear listͷ������first image */
		obj = list_entry(node, struct exe_objects , linear_list);
		
		/* Do relocation */
		handle_bss(obj);
		elf_relocation(&obj->exe_desc, &obj->exe_desc, &obj->user_ctx);

#ifndef __mips__
		/* Set lazy linker */
		elf_set_lazy_linker(&obj->exe_desc, so_lazy_link, &obj->user_ctx);
#endif
	}

	return 0;
}

/**
	@brief startup image
*/
static int startup()
{
	int id = 0;

	struct exe_objects * obj;
	struct list_head * node;
	int (*start)(unsigned long);
	int (*start_first)(unsigned long);
	
	list_for_each(node, &objs_linear_list)
	{	
		/* ��linear list��ȡ��image��linear listͷ������first image������ִ���ļ� */
		obj = list_entry(node, struct exe_objects , linear_list);

		if (elf_get_startup(&obj->exe_desc, &obj->user_ctx, (unsigned long *)&start) == false)
		{
			return EINVAL;
		}		
		/* linear listͷ����ʾ��ִ���ļ� */
		if (0 == id++)
		{
			start_first = start;
		}
		else
		{
// 			char str[32];
// 
// 			str[h2c(str, (unsigned long)obj->user_ctx.base)] = 0;
// 			early_print(str);early_print(" ");
			/* start up signal */
			start(0);		
		}
	}
	
	/* ��ִ���ļ�Ҫ��ִ���������������ִ�� */
	return start_first((unsigned long)cmdline_buffer);						// ���������е�main
}

/**
	@brief װ���ƶ��ļ�����������main����
 */
static int dl_build_image_context(xstring first_name)
{
	struct exe_objects * p;

	/* Load the first image which normally is an EXE */
	p = load_image(first_name);
	if (!p)
	{
		early_print(DBG_PREFIX"װ�ؿ�ִ���ļ�ʧ��.\n");
		return ENOENT;
	}
	
	early_print("Loading dependency...\n");
	load_dependencies();

	early_print("Relocating...\n");
	relocation();

	early_print("Starting up...\n");
	return startup();
}

/**
	@brief get a symbol
*/
bool __weak ki_get_symbol(struct elf_context * elf, xstring name, unsigned long * address, bool search_other)
{
	bool ret = false;
	struct exe_objects * what = container_of(elf, struct exe_objects, exe_desc);
	
	/* ���������Ķ�̬�� */
	if (search_other == true)
	{	
		struct dependency_list * node;

		if (what->d_list == NULL)
			return false;

		for (node = what->d_list->next; node != NULL; node = node->next)
		{
//  		early_print("Finding symbol ");
//  		early_print(name);
//  		early_print(" at ");
//  		early_print(node->obj->name);
			ret = elf_resolve_symbol_by_name(&node->obj->exe_desc, &node->obj->user_ctx, name, address);
			if (ret == true) break;	
//			early_print("failed\n");
		}
	}
	else
	{
		ret = elf_resolve_symbol_by_name(&what->exe_desc, &what->user_ctx, name, address);
	}
	
	return ret;
}

/**
	@brief Entry
*/
void dl_dynamic_linker()
{
	struct sysreq_process_startup st;
	xstring exe_name, path_end;
	int ret = -1, i;

	/* 
		Get the cmd line, 
		���а����˾���·��������ģ�����û�취�ж��û����������·������
		����Ҫ��ȡ����·���������뵽��̬�������б�ͬʱ����Ϊ��ǰ����Ŀ¼��
	*/
	st.base.req_id		= SYS_REQ_KERNEL_PROCESS_STARTUP;
	st.main_function	= (unsigned long)dl_dynamic_linker;
	st.cmdline_buffer	= cmdline_buffer;
	st.func				= SYSREQ_PROCESS_STARTUP_FUNC_START;							// Get command line
	system_call(&st);
	
	/* �����ִ���ļ� */
	exe_name = strchr(cmdline_buffer, ' '/*space*/);
	if (!exe_name)
		exe_name = strchr(cmdline_buffer, '	'/*tab*/);
	if (!exe_name)
		goto startup_end;
	exe_name++;																			// Escape blank
	
	/* ��ȡ��ִ���ļ�·�����Ժ����������̵߳ĵ�ǰ·�� */
	path_end = strchr(exe_name, ' '/*space*/);
	if (!path_end)
		path_end = strchr(exe_name, '	'/*tab*/);
	if (!path_end)
	{
		/* The exe name is the last string in the cmdline */
		path_end = exe_name + strlen(exe_name);
	}
	for (i = sizeof(xchar); ; i++)
	{
		if (path_end[-i] == '/' || path_end[-i] == '\\')
			break;
		if (path_end == exe_name)
			goto startup_end;
	}
	path_end[-i]		= 0;
	st.base.req_id		= SYS_REQ_KERNEL_PROCESS_STARTUP;
	st.cmdline_buffer	= exe_name;
	st.func				= SYSREQ_PROCESS_STARTUP_FUNC_SET_PATH;							// Get command line
	system_call(&st);
	path_end[-i]		= '/';
	
	/* ��ʼװ�� */
	INIT_LIST_HEAD(&objs_linear_list);
	ret = dl_build_image_context(exe_name);

	/* ����Ӧ�ò������е����� */
startup_end:
	st.func				= SYSREQ_PROCESS_STARTUP_FUNC_END;								// End of process
	st.ret_code			= ret;
	system_call(&st);
}
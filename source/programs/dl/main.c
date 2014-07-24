/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *
 *   动态连接器
 */

#include <types.h>
#include <list.h>
#include <errno.h>
#include <string.h>
#include <compiler.h>

#include <kernel/ke_srv.h>

#include "sys/ke_req.h"
#include "cl_fname.h"
#include "../../libs/elf2/elf.h"

/************************************************************************/
/* Note:                                                                */
/* Must not in BSS, ld.sys not handled BSS by kernel                    */
/************************************************************************/

/* 启动时，我们最多能装载这么多的image，因为我们还没有分配器 */
#define DL_MAX_STATIC_IMAGE 512
#define DL_ALIAS_NAME_LENGTH 128/* TODO 以后最好能动态分配*/
#define DL_RUNTIME_LIBRARY_NAME "grid.so"

/* 依赖表，用于串联对象的直接依赖 */
struct dependency_list
{
	struct list_head list;
	struct image *obj;
};

struct elf_context
{
	char data[512];
};

struct image
{
	/* Name points normally to the string in image file */
	xchar *name;
	struct list_head linear_list, dep_list;
	struct elf_context exe_desc;
	struct elf_user_ctx user_ctx;
	ke_handle handle;
	
	/* status */
	int ref;
	int mode;
	char relocated, dependency_loaded;
};
#define DBG_PREFIX "DL装载器："

struct dl_structure
{
	/* 线性表，用于串联所有可执行文件 */
	struct list_head image_list_head;
	
	/* The startup cmdline */
	xchar cmdline_buffer[SYSREQ_PROCESS_STARTUP_MAX_SIZE];
	
	/* Early Allocator */
	struct image static_image_pool[DL_MAX_STATIC_IMAGE];
	char static_image_usage[DL_MAX_STATIC_IMAGE];
	struct dependency_list static_dependency_pool[DL_MAX_STATIC_IMAGE];
	char static_dependency_usage[DL_MAX_STATIC_IMAGE];
	
	/* Temp Usage */
	xchar *exe_name_end;
	xchar exe_name_end_char;
	char normal_mode;
	
} dl = {{(void*)1}};

static void *_map_exe_file(xstring name, int *size)
{
	void *base;
	struct sysreq_process_ld req;

	req.base.req_id		= SYS_REQ_KERNEL_PROCESS_HANDLE_EXE;
	req.name			= name;
	req.function_type	= SYSREQ_PROCESS_MAP_EXE_FILE;
	base = (void*)system_call(&req);
	
	*size = req.context_length;
	return base;
}

static void _unmap_exe_file(void *base)
{
	struct sysreq_process_ld req;
	
	req.base.req_id		= SYS_REQ_KERNEL_PROCESS_HANDLE_EXE;
	req.name			= base;
	req.function_type	= SYSREQ_PROCESS_UNMAP_EXE_FILE;
	system_call(&req);
}

static bool _inject_exe_object(xstring name, void *ctx, int ctx_size)
{
	struct sysreq_process_ld req;
	
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
	struct sysreq_process_printf req;
	req.base.req_id = SYS_REQ_KERNEL_PRINTF;
	req.string = string;
	system_call(&req);
}

static int h2c(char *p,unsigned long hex)
{	
	int i;
	int j=((sizeof(void*))*8)/4;						//*8 means bits,/4 means a char represent 4 bits
	unsigned long old=hex;

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
	@brief Allocate a private context for an image to store the basic elf info
 */
static struct image *_allocate_static_exe()
{
	int i;
	
	/* Loop the array to get a free one */
	for (i = 0; i < DL_MAX_STATIC_IMAGE; i++)
	{
		if (dl.static_image_usage[i] == 0)
		{
			dl.static_image_usage[i] = 1;
			return &dl.static_image_pool[i];
		}
	}
	
	return NULL;
}

/**
	@brief Allocate a private dependency_list
 */
static struct dependency_list *_allocate_static_d_list()
{
	int i;
	
	/* Loop the array to get a free one */
	for (i = 0; i < DL_MAX_STATIC_IMAGE; i++)
	{
		if (dl.static_dependency_usage[i] == 0)
		{
			dl.static_dependency_usage[i] = 1;
			return &dl.static_dependency_pool[i];
		}
	}
	
	return NULL;
}

static bool check_image_precicely(struct image *image, const char *fname)
{
	/*
	 找到如此匹配的名字，然后去文件系统验证到底是不是对的。
	 以此解决各种变态路径问题，只有文件系统能确定路径问题。
	 */
	//TODO
	return true;
}

/**
	@brief get image from linear_list
*/
static struct image *_get_obj_from_linear_list(char *name)
{
	struct image *image;
	const char *pure_dst_name, *pure_src_name;
	
	pure_src_name = cl_locate_pure_file_name(name);
	list_for_each_entry(image, &dl.image_list_head, linear_list)
	{
		/* 一些变态的动态库中还是有"../../abc.so"这样的依赖 */
		pure_dst_name = cl_locate_pure_file_name(image->name);
		if (!strcmp(pure_dst_name, pure_src_name))
		{
			/* ok，在这里，我们只能过滤到这个地步，精确匹配交给文件系统 */
			if (check_image_precicely(image, name) == true)
			{
				image->ref++;
				return image;
			}
		}
	}
	
	return NULL;
}

/**
	@brief image insert to dependency list
*/
static struct dependency_list *add_obj_to_dependency_list(struct image *dependency, struct image *obj)
{
	struct dependency_list *node = _allocate_static_d_list();
	if (!node)
		return NULL;
	node->obj = dependency;
	list_add_tail(&node->list, &obj->dep_list);
	return node;
}

/**
	@brief 如果可执行文件曾经被装在过，则直接用历史的，否则新装载
 
	@note
		name 必须是文件的绝对路径
*/
static ke_handle load(xstring name, struct image *obj, struct image **aliased)
{
	struct sysreq_process_ld req;
	ke_handle image_handle;
	int new_loaded = 0;
	char alias_name[DL_ALIAS_NAME_LENGTH];
	
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
	}
	else if (new_loaded == 0)
	{
		int file_size;
		char *file;

		file = _map_exe_file(name, &file_size);
		if (!file)
			goto err;
		if (elf_analyze(file, file_size, NULL, &obj->exe_desc) == false)
		{
			/* Alias? */
			if (file[0] == 'a' && file[1] == 'l' && file[2] == 'i' && file[3] == 'a')
			{
				char *end_pos;
				
				if ((name = strchr(file, ' ')) == NULL)
					goto end1;
				name++;
				if ((end_pos = strchr(name, ' ')) == NULL)
					goto end1;
					
				if (end_pos - name < DL_ALIAS_NAME_LENGTH)
				{
					memcpy(alias_name, name, end_pos - name);
					alias_name[end_pos - name] = 0;
					name = alias_name;
							
					/* May already in the list */
					if ((*aliased = _get_obj_from_linear_list(name)) != NULL)
						return KE_INVALID_HANDLE;
							
					goto open_again;
				}
			}
			
			goto end1;
		}
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

static struct image *load_image(xstring name)
{
	struct image *p, *aliesed = NULL;

	p = _allocate_static_exe();
	if (!p)
		goto err;
	
	//TODO allocate dynamic exe

	/* Load from disk */
	p->handle = load(name, p, &aliesed);
	if (p->handle == KE_INVALID_HANDLE)
		goto err;

	/* 如果打开失败，可以尝试动态库列表中的路径去打开可执行文件 */
	//TODO

	p->name = name;
	p->ref = 1;
	INIT_LIST_HEAD(&p->dep_list);
	
	/* 将对象加入依赖线性表linear list中 */
	list_add_tail(&p->linear_list, &dl.image_list_head);
	
	return p;
err:
	if (p)
	{
		//TODO: DELETE P;
	}
	
	if (aliesed)
		return aliesed;
	return NULL;
}

static void unload_image(struct image *image)
{
	struct sysreq_process_ld req;

	if (image->ref > 1)
	{
		image->ref--;
		return;
	}
	
	req.base.req_id		= SYS_REQ_KERNEL_PROCESS_HANDLE_EXE;
	req.function_type	= SYSREQ_PROCESS_CLOSE_EXE;
	req.name			= (void*)image->user_ctx.base;/* By address */
	system_call(&req);
}

/**
	@brief load a dependent image 

	@return
		0 : end;
		1 : OK;
		-1: Error;
*/
static int load_dependency(struct image *p, int id)
{
	int real_load;
	struct image *obj;
	xstring name;
	//char str[32];

	/* Get the name of this id */
	name = elf_get_needed(&p->exe_desc, id, &p->user_ctx);
	if (!name)
		goto end;
	//str[h2c(str, name)] = 0;
	//early_print("load_dependency ");early_print(str); early_print(name); early_print(".\n");

	/* 如果对象已存在于linear list中，就没必要再执行load_image获取 */
	real_load = 0;
	obj = _get_obj_from_linear_list(name);
	if (!obj)
	{
		//early_print("LOading from disk... ");
		obj = load_image(name);
		if (!obj)
			goto err;
		real_load = 1;
	//	early_print("OK\n");

	}
	if (add_obj_to_dependency_list(obj, p) == NULL)
		goto err;
	
	return 1;
end:
	return 0;
err:
	if (real_load)
		unload_image(obj);
	return -1;
}

/**
	@brief load all dependent images for image 
*/
static int load_dependencies()
{
	int id, ret = 0;
	struct image * obj;
	struct list_head * node;
	
	/* 第一次循环时，node代表的first image */
	list_for_each(node, &dl.image_list_head)
	{	
		/* 从linear list中取出image，linear list头结点代表first image */
		obj = list_entry(node, struct image , linear_list);
		if (obj->dependency_loaded)
			continue;
// 		early_print("Loading dep for ");early_print(obj->name);early_print(".\n");

		/* 查找image的依赖 */
		for (id = 0; ; id++)
		{
			ret = load_dependency(obj, id);

			/* Error */
			if (ret < 0)
			{
#if 1
				char *dep;
				
				dep = elf_get_needed(&obj->exe_desc, id, &obj->user_ctx);
				early_print("Loading dep ");
				early_print(dep);

				early_print(" 失败，但是继续.\n");
				//return ret;
#endif
			}
			/* End */
			else if (ret == 0)
			{
				obj->dependency_loaded = 1;
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
	struct image *object = container_of(p, struct image, exe_desc);

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

static void handle_bss(struct image *exe)
{
	int i;
	
	unsigned long base = 0;
	struct elf_segment seg;
	volatile unsigned char *last_byte;

	for (i = 0; ; i++)
	{
		if (elf_read_segment(&exe->exe_desc, &seg, i) == false)
			break;
		if (seg.fsize == seg.vsize)
			continue;
	
		/* 自动地址吗？ */
		if (elf_get_mapping_base(&exe->exe_desc, NULL, NULL) == NULL)
			base = exe->user_ctx.base;

		/* Get the address of the last byte of valid file mapping */
		last_byte = (unsigned char*)(seg.fsize - 1 + seg.vstart) + base;

		/* Reading the last byte cause exe refill exception of sharing, writing cause cow */
		*last_byte = *last_byte;

		/* Set the space after valid_size to zero to clean the "topmost" BSS part */
		memset((void*)last_byte + 1, 0, seg.align - ((unsigned long)(last_byte + 1) & (seg.align - 1)));
	}
}

/**
	@brief relocate images
*/
static int relocation()
{
	struct image *image;
	struct list_head *node;
	
	/* 第一次循环时，node代表的first image */
	list_for_each(node, &dl.image_list_head)
	{	
		/* 从linear list中取出image，linear list头结点代表first image */
		image = list_entry(node, struct image , linear_list);
		if (image->relocated)
			continue;
		image->relocated = 1;
		handle_bss(image);
		elf_relocation(&image->exe_desc, &image->exe_desc, &image->user_ctx);

#ifdef SET_LAZY
#ifndef __mips__
		/* Set lazy linker */
		elf_set_lazy_linker(&image->exe_desc, so_lazy_link, &image->user_ctx);
#else
#warning "lazy linker not set"
#endif
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

	struct image *obj;
	struct list_head *node;
	int (*start)(unsigned long);
	int (*init)(void*);
	int (*start_first)(unsigned long) = NULL;

	/* 恢复参数列表 */
	*dl.exe_name_end = dl.exe_name_end_char;

	list_for_each(node, &dl.image_list_head)
	{
		obj = list_entry(node, struct image , linear_list);
		if (elf_get_startup(&obj->exe_desc, &obj->user_ctx, (unsigned long *)&start,
							(unsigned long*)&init, NULL) == false)
		{
			return EINVAL;
		}
		
		/* linear list头结点表示可执行文件 */
		if (0 == id++)
		{
			start_first = start;
		}
		else
		{
// 			char str[32];

// 			str[h2c(str, (unsigned long)obj->user_ctx.base)] = 0;
// 			early_print(str);early_print(" ");
			
			if (init)
			{
				if (!strcmp(obj->name, DL_RUNTIME_LIBRARY_NAME))
					init(&dl);
				else
					init(0);
			}
			
			/* start up signal */
			start(0);		
		}
	}
	
	/* 可执行文件要在执行完所有依赖库后执行 */
	if (!start_first)
		return ENOENT;
	return start_first((unsigned long)dl.cmdline_buffer);						// 传递命令行到main
}

/**
	@brief 装载制定文件，并且启动main函数
 */
static int dl_build_image_context(xstring first_name)
{
	struct image * p;

	/* Load the first image which normally is an EXE */
	p = load_image(first_name);
	if (!p)
	{
		early_print(DBG_PREFIX"装载可执行文件失败.\n");
		return ENOENT;
	}
	
	//early_print("Loading dependency...\n");
	load_dependencies();

	//early_print("Relocating...\n");
	relocation();

	//early_print("Starting up...\n");
	return startup();
}

#include <dlfcn.h>
/**
	@brief get a symbol
*/
bool ki_get_symbol(struct elf_context * elf, const char *name, unsigned long * address, bool search_other)
{
	bool ret = false;
	struct image *what = container_of(elf, struct image, exe_desc);
	
	/* 搜索依赖的动态库 */
	if (search_other == true)
	{	
		struct dependency_list *node;
		struct image *every_one;

		if (what->mode == RTLD_GLOBAL)
		{
			list_for_each_entry(every_one, &dl.image_list_head, linear_list)
			{
				if (every_one == what)
					continue;
				ret = elf_resolve_symbol_by_name(&every_one->exe_desc,
												 &every_one->user_ctx,
												 name, address);
				if (ret == true) break;
			}
		}
		else
		{
			list_for_each_entry(node, &what->dep_list, list)
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
		其中包括了绝对路径（必须的，否则没办法判断用户程序的启动路径），
		我们要提取绝度路径，并加入到动态库搜索列表，同时设置为当前工作目录。
	*/
	st.base.req_id		= SYS_REQ_KERNEL_PROCESS_STARTUP;
	st.main_function	= (unsigned long)dl_dynamic_linker;
	st.cmdline_buffer	= dl.cmdline_buffer;
	st.func				= SYSREQ_PROCESS_STARTUP_FUNC_START;							// Get command line
	system_call(&st);
	
	/* 分离可执行文件 */
	exe_name = dl.cmdline_buffer;
	
	/* 获取可执行文件路径，以后用于设置线程的当前路径 */
	path_end = strchr(exe_name, ' '/*space*/);
	if (!path_end)
		path_end = strchr(exe_name, '	'/*tab*/);
	if (!path_end)
	{
		/* The exe name is the last string in the cmdline */
		path_end = exe_name + strlen(exe_name);
	}
	dl.exe_name_end = path_end;

	for (i = sizeof(xchar); ; i++)
	{
		if (path_end[-i] == '/' || path_end[-i] == '\\')
			break;
		if (&path_end[-i] == exe_name)
			goto startup_end;
	}
	path_end[-i]		= 0;
	st.base.req_id		= SYS_REQ_KERNEL_PROCESS_STARTUP;
	st.cmdline_buffer	= exe_name;
	st.func				= SYSREQ_PROCESS_STARTUP_FUNC_SET_PATH;							// Get command line
	system_call(&st);
	path_end[-i]		= '\\';
	
	/* 开始装载 */
	dl.exe_name_end_char = *dl.exe_name_end;
	dl.exe_name_end[0] = 0;																// 可执行文件名和参数之间的空隙
	INIT_LIST_HEAD(&dl.image_list_head);
	
	ret = dl_build_image_context(exe_name);

	/* 程序正常应该不会运行到这里 */
startup_end:
	st.func				= SYSREQ_PROCESS_STARTUP_FUNC_END;								// End of process
	st.ret_code			= ret;
	system_call(&st);
}
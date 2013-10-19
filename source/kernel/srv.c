/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin（82828068@qq.com）
 *   内核 用户层的接口
 */

#include <linkage.h>

#include <section.h>
#include <process.h>
#include <sync.h>
#include <exe.h>
#include <handle.h>

#include "string.h"
#include <asm/abicall.h>

#include <kernel/ke_srv.h>

#include "../source/subsystem/fs/include/fss.h"
#include "../source/libs/grid/include/sys/ke_req.h"

bool ke_validate_user_buffer(void * buffer, size_t size, bool rw)
{
	return true;
}

static struct ko_section *map_file(struct ko_process *to, xstring name, page_prot_t prot, unsigned long *map_size)
{
	struct ko_section *ks_file;
	unsigned long size;
	void *fp;
	
	fp = fss_open(kt_current()->current_dir, name);
	if (!fp) goto end;
	
	*map_size = size = fss_get_size(fp);
	ks_file = ks_create(to, KS_TYPE_FILE, NULL, size, prot);
	if (!ks_file)
		goto err1;
	ks_file->priv.file.file = fp;
	return ks_file;
	
err1:
	fss_close(fp);
end:
	return NULL;
}

/************************************************************************/
/* Process                                                              */
/************************************************************************/
/**
 @brief New process send a startup signal
 
 We copy the startup cmdline to the process
 */
static void process_startup(struct sysreq_process_startup * req)
{
	xstring __user cmdline = req->cmdline_buffer;
	
	if (req->func == SYSREQ_PROCESS_STARTUP_FUNC_START)
	{
		/* The user buffer can be written ? */
		if (ke_validate_user_buffer(cmdline, SYSREQ_PROCESS_STARTUP_MAX_SIZE, true) == false)
			return;
		memcpy(cmdline, (void*)(kt_arch_get_sp0(kt_current()) - KT_ARCH_THREAD_CP0_STACK_SIZE),
			   SYSREQ_PROCESS_STARTUP_MAX_SIZE);
		printk("cmdline is %s.\n", cmdline);
	}
	else if (req->func == SYSREQ_PROCESS_STARTUP_FUNC_SET_PATH)
	{
		/* The user buffer can be written ? */
		if (ke_validate_user_buffer(cmdline, SYSREQ_PROCESS_STARTUP_MAX_SIZE, false) == false)
			return;
		printk("current dir = %s.\n", cmdline);
		kt_current()->current_dir = fss_open(NULL, cmdline);
	}
	else if (req->func == SYSREQ_PROCESS_STARTUP_FUNC_END)
	{
		/* Killing main thread will trigger the process destroying */
		kt_delete_current();
	}
}

/**
	@brief dynamic linker ops
 */
static ke_handle process_ld(struct sysreq_process_ld * req)
{
	ke_handle handle;
	xstring module_name;
	
	switch (req->function_type)
	{
		case SYSREQ_PROCESS_OPEN_EXE:
		{
			struct ko_exe *image;
			
			module_name = req->name;
			
			if (ke_validate_user_buffer(req->context, req->context_length, true) == false)
				goto ld_0_err;
			if (ke_validate_user_buffer(module_name, strlen(module_name), false) == false)
				goto ld_0_err;
			if (ke_validate_user_buffer(&req->map_base, sizeof(req->map_base), true) == false)
				goto ld_0_err;
			
			image = kp_exe_open_by_name(KP_CURRENT(), module_name);
			if (!image)
				goto ld_0_err;
			if (kp_exe_copy_private(image, req->context, req->context_length) == false)
				goto ld_0_err1;
			
			/* Create handle for this image */
			handle = ke_handle_create(image);
			if (handle == KE_INVALID_HANDLE)
				goto ld_0_err1;

			return handle;
			
		ld_0_err1:
			//TODO: close the object
		ld_0_err:
			return KE_INVALID_HANDLE;
		}
		
		/* Return the map base */
		case SYSREQ_PROCESS_MAP_EXE_FILE:
		{
			struct ko_section *file_section;
			
			module_name = req->name;
			
			if (ke_validate_user_buffer(module_name, strlen(module_name), false) == false)
				goto map_0_err;
			file_section = map_file(KP_CURRENT(), module_name, KM_PROT_READ, &req->context_length);
			if (file_section == NULL)
				goto map_0_err;
			
			return (ke_handle)file_section->node.start;
			
		map_0_err:
			return 0;
		}
		
		/* 删除本地map的文件 */
		case SYSREQ_PROCESS_UNMAP_EXE_FILE:
		{
			void *base = req->name;
			TRACE_UNIMPLEMENTED("");
		}
		break;
		
		/* Add a new exe object, return bool */
		case SYSREQ_PROCESS_ENJECT_EXE:
		{
			struct ko_exe *image;
			void *ctx;
			int ctx_size;
			unsigned long size;
			struct ko_section *file_section;
			
			ctx_size		= req->context_length;
			ctx				= req->context;
			module_name		= req->name;
			
			if (ke_validate_user_buffer(ctx, ctx_size, false) == false)
				goto ld_3_err;
			if (ke_validate_user_buffer(module_name, strlen(module_name), false) == false)
				goto ld_3_err;
			if (ctx_size > kp_exe_get_context_size())
				goto ld_3_err;
			if ((file_section = map_file(kp_get_file_process(), module_name, KM_PROT_READ, &size)) == NULL)
				goto ld_3_err;
			if (kp_exe_create_from_file(module_name, file_section, ctx) == NULL)
				goto ld_3_err;
			
			return true;
			
		ld_3_err:
			return false;
		}
		default:
			break;
	}

	return 0;
}

/************************************************************************/
/* MISC                                                                 */
/************************************************************************/

/**
*/
static void kernel_printf(struct sysreq_process_printf * req)
{
	printk("%s", req->string);
}

/************************************************************************/
/* MEMORY                                                               */
/************************************************************************/
static ke_handle memory_virtual_alloc(struct sysreq_memory_virtual_alloc * req)
{
	unsigned long base, sz, type;

	ke_handle handle;
	struct ko_section *ks_virtual;
	
	if (req->name)
	{
		type = KS_TYPE_SHARE;
		TRACE_UNIMPLEMENTED("命名内存空间还没有支持");
		goto err0;
	}
	else
		type = KS_TYPE_PRIVATE;
	
	base	= req->base_address;
	sz		= req->size;
	ks_virtual = ks_create(KP_CURRENT(), type, base, sz, req->mem_prot);
	if (!ks_virtual) goto err0;
	handle = ke_handle_create(ks_virtual);
	if (handle == KE_INVALID_HANDLE) goto err1;
	
	/* Fill output */
	req->out_base = ks_virtual->node.start;
	req->out_size = ks_virtual->node.size;
	return handle;
	
err1:
	TODO("分配内存段错误，须回收");
err0:
	return KE_INVALID_HANDLE;
}

/**
	@brief 内核自己的服务
*/
static void * kernel_entry[SYS_REQ_KERNEL_MAX - SYS_REQ_KERNEL_BASE];
static unsigned long kernel_srv(unsigned long req_id, void * req)
{
	unsigned long (*func)(void * req);

	func = kernel_entry[req_id];
	return func(req);
}

/**
	@brief 系统调用总入口
*/
const static struct ke_srv_info ke_srv = {
	.name = "Native 内核服务",
	.service_id_base = SYS_REQ_KERNEL_BASE,
	.request_enqueue = kernel_srv,
};
const static struct  ke_srv_info * ke_interface_v2[KE_SRV_MAX];
asmregparm unsigned long arch_system_call(unsigned long * req)
{
	unsigned long req_id = *req;
	unsigned long ret;
	int clasz = KE_SRV_GET_CLASZ(req_id);
	unsigned long (*func)(unsigned long func_id, void * req);

	/* Sanity check */
	if (ke_interface_v2[clasz] == NULL)
	{
		printk("syscall at class %d, req id %d: 没有注册对应的服务处理接口。\n", clasz, KE_SRV_GET_FID(req_id));
		return -1;
	}
	func = ke_interface_v2[clasz]->request_enqueue;
	ret	= ka_call_dynamic_module_entry(func, KE_SRV_GET_FID(req_id), req);

	/* But is a dead thread? */
	//TODO

	return ret;
}

bool ke_srv_register(const struct ke_srv_info * info)
{
	int id = KE_SRV_GET_CLASZ(info->service_id_base);

	/* Set the SRV info pointer, which is the root of the subsystem */
	if (ke_interface_v2[id])
	{
		printk("内核服务 %s 所在的IDBASE已经被注册，无法再次注册。\n", info->name);
		return false;
	}
	ke_interface_v2[id] = info;

	return true;

err1:
	ke_interface_v2[id] = NULL;
	return false;
}

void ke_srv_init()
{
//	kernel_entry[SYS_REQ_KERNEL_THREAD_CREATE - SYS_REQ_KERNEL_BASE]	= (void*) thread_create;
	///kernel_entry[SYS_REQ_KERNEL_THREAD_WAIT - SYS_REQ_KERNEL_BASE]		= (void*) thread_wait;
	//kernel_entry[SYS_REQ_KERNEL_PROCESS_CREATE - SYS_REQ_KERNEL_BASE]	= (void*) process_create;
	kernel_entry[SYS_REQ_KERNEL_PROCESS_STARTUP - SYS_REQ_KERNEL_BASE]		= (void*) process_startup;
	kernel_entry[SYS_REQ_KERNEL_PROCESS_HANDLE_EXE - SYS_REQ_KERNEL_BASE]	= (void*) process_ld;
	//kernel_entry[SYS_REQ_KERNEL_WAIT - SYS_REQ_KERNEL_BASE]				= (void*) process_wait;


	/* Misc */
	kernel_entry[SYS_REQ_KERNEL_PRINTF - SYS_REQ_KERNEL_BASE]			= (void*) kernel_printf;
	//kernel_entry[SYS_REQ_KERNEL_MISC_DRAW_SCREEN - SYS_REQ_KERNEL_BASE] = (void*) misc_draw_screen;

	/* Memory part */
	kernel_entry[SYS_REQ_KERNEL_VIRTUAL_ALLOC - SYS_REQ_KERNEL_BASE]	= (void*) memory_virtual_alloc;

	ke_srv_register(&ke_srv);
}

/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin (82828068@qq.com)
 *	 系统请求信息数据
 */

#ifndef KE_SYS_REQ
#define KE_SYS_REQ

#include <types.h>
#include <kernel/ke_memory.h>
#include "syscall.h"

/************************************************************************/
/* KERNEL FUNCTION of SYSREQ                                            */
/************************************************************************/
struct sysreq_thread_create
{
	/* Input */
	struct sysreq_common base;
	xstring name;
	void * wrapper, * entry;
	unsigned long para;
	bool run;

	/* Output */
	ke_handle thread;
};

struct sysreq_thread_delete
{
	/* Input */
	struct sysreq_common base;
	ke_handle thread;
};

struct sysreq_thread_wait
{
	/* INPUT */
	struct sysreq_common base;
	unsigned int ms;
};

/**
	@brief Create process
*/
struct sysreq_process_create
{
	/* Input */
	struct sysreq_common base;
	xstring name, image;

	/* Output */
	ke_handle process;
};

struct sysreq_process_startup
{
	/* INPUT */
	struct sysreq_common base;
	xstring cmdline_buffer;
	unsigned long main_function;
	int func;																		// 0 is get cmdline; 1 is set current path; 2 is end of process
	int ret_code;																	// the return code of the the process
#define SYSREQ_PROCESS_STARTUP_FUNC_START		0
#define SYSREQ_PROCESS_STARTUP_FUNC_SET_PATH	1									// Set working path
#define SYSREQ_PROCESS_STARTUP_FUNC_END			2
#define SYSREQ_PROCESS_STARTUP_MAX_SIZE			512									// Max size of the user buffer to store the cmdline
};

struct sysreq_process_ld
{
	/* INPUT */
	struct sysreq_common base;
	xstring name;
	size_t	context_length;
	int	function_type;
	void *context;

	/* Output */
	unsigned long map_base;															// Mapped base of this module on user space

#define SYSREQ_PROCESS_OPEN_EXE			0
#define SYSREQ_PROCESS_MAP_EXE_FILE		1
#define SYSREQ_PROCESS_UNMAP_EXE_FILE	2
#define SYSREQ_PROCESS_ENJECT_EXE		3
};

/************************************************************************/
/* MEMORY                                                               */
/************************************************************************/
struct sysreq_memory_virtual_alloc
{
	/* INPUT */
	struct sysreq_common	base;
	xstring					name;													// The segment name
	unsigned long			base_address;
	size_t					size;
	page_prot_t				mem_prot;												//
	
	/* Output */
	unsigned long			out_base;
	size_t					out_size;
};

void *sys_vmalloc(xstring name, void *addr, size_t len, int prot);

/************************************************************************/
/* misc                                                                 */
/************************************************************************/
struct sysreq_misc_draw_screen
{
	/* INPUT */
	struct sysreq_common base;
	int x, y;
	union
	{
		struct __draw_screen_pixel__
		{
			unsigned int clr;
		} pixel;
		struct __draw_screen_bitmap__
		{
			int width, height;
			int bpp;
			void * __user buffer;
		} bitmap;
		
		struct __draw_screen_resolution__
		{
			int width, height, bpp;
		} resolution;
	};
	int type;
#define SYSREQ_MISC_DRAW_SCREEN_PIXEL	0
#define SYSREQ_MISC_DRAW_SCREEN_BITMAP	1
#define SYSREQ_MISC_DRAW_GET_RESOLUTION	2
};

struct sysreq_process_printf
{
	/* INPUT */
	struct sysreq_common base;
	void * string;
};

#endif

#ifndef KE_SYS_REQ
#define KE_SYS_REQ

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
	void * context;

#define SYSREQ_PROCESS_LD_OPEN 0
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
	unsigned long			mem_prot;												// 
	
	/* Output */
	unsigned long			out_base;
	size_t					out_size;
};

/************************************************************************/
/* misc                                                                 */
/************************************************************************/
struct sysreq_misc_set_pixel
{
	/* INPUT */
	struct sysreq_common base;
	int x, y;
	unsigned int clr;
};

struct sysreq_process_printf
{
	/* INPUT */
	struct sysreq_common base;
	void * string;
};

#endif

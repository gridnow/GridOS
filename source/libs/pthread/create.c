#include <pthread.h>

#include "sys/ke_req.h"

#ifdef __i386__
#define asmregparm __attribute__((regparm(3)))
#else
#define asmregparm
#endif

static asmregparm void * thread_wrap(void *(PTCDECL *start) (void *), void *arg)
{
	struct sysreq_process_startup req;

// 	printf("User level thread wrapper, start is %x, arg %x\n", start, arg);
	start(arg);

	/* Kill current thread, like the ld.sys */
	req.base.req_id		= SYS_REQ_KERNEL_PROCESS_STARTUP;
	req.func			= SYSREQ_PROCESS_STARTUP_FUNC_END;
	system_call(&req);
}

static ke_handle create_thread(void *entry, void * arg, bool run)
{
	struct sysreq_thread_create req;
	req.base.req_id = SYS_REQ_KERNEL_THREAD_CREATE;
	req.wrapper		= thread_wrap;
	req.entry		= entry;
	req.para		= (unsigned long)arg;
	req.run			= run;
	system_call(&req);
	return req.thread; 
}

/**
	@brief Create a thread
*/
int pthread_create(pthread_t * tid, const pthread_attr_t * attr, void *(PTCDECL *start) (void *), void *arg)
{
	/* Create the thread from system */
	ke_handle h = create_thread(start, arg, true);
	if (tid) *tid = h;

	/* Set return */
	if (h == KE_INVALID_HANDLE)
		return EAGAIN;

	return 0;
}
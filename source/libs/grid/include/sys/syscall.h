/**
	The Grid Core Library
	
	Yaosihai
*/

/**
*   不同的硬件平台有不同的系统请求方法
*/
#ifndef SYSTEM_CALL_H
#define SYSTEM_CALL_H

#ifdef _MSC_VER
#include <ke_srv.h>
#else
#include <kernel/ke_srv.h>
#endif

#if defined (__i386__)
static inline unsigned long system_call(void *  req_package)
{
	unsigned long ret;

	/* Must clobber edx,ecx temp register, because the system handler does not save it */
	__asm__ __volatile__ (
		"int $0x87		\n\t"
		:"=a" (ret) :"a" (req_package):"edx", "ecx", "memory"
		);
	return ret;
}
#elif defined (__mips__)
/* The n64, n32 Use the same registers layout, we do not support o32 layout */
#define __SYSCALL_CLOBBERS "$1", "$3", "$10", "$11", "$12", "$13", \
	"$14", "$15", "$24", "$25", "hi", "lo", "memory"

#define internal_syscall1(arg1)		\
({ 									\
	long _sys_result;				\
									\
	{								\
	register long __v0 asm("$2") ;	\
	register long __a0 asm("$4") = (long) arg1; 	\
	__asm__ volatile (								\
	".set\tnoreorder\n\t"							\
	"syscall\n\t"									\
	".set reorder"									\
	: "=r" (__v0)									\
	: "r" (__a0)									\
	: __SYSCALL_CLOBBERS);							\
	_sys_result = __v0;								\
	}												\
	_sys_result;									\
})

static inline unsigned long system_call(void *  req_package)
{
	return internal_syscall1(req_package);
}
#endif

/************************************************************************/
/* KERNEL calling                                                       */
/************************************************************************/
#define KERNEL_REQ_BASE						0
#define SYS_REQ_KERNEL_BASE					KE_SRV_MAKE_REQ_NUM(KERNEL_REQ_BASE, 0)
#define SYS_REQ_KERNEL_PROCESS_CREATE		(SYS_REQ_KERNEL_BASE + 0)
#define SYS_REQ_KERNEL_THREAD_CREATE		(SYS_REQ_KERNEL_BASE + 1)
#define SYS_REQ_KERNEL_THREAD_WAIT			(SYS_REQ_KERNEL_BASE + 2)
#define SYS_REQ_KERNEL_PROCESS_STARTUP		(SYS_REQ_KERNEL_BASE + 3)
#define SYS_REQ_KERNEL_PRINTF				(SYS_REQ_KERNEL_BASE + 4)
#define SYS_REQ_KERNEL_PROCESS_HANDLE_EXE	(SYS_REQ_KERNEL_BASE + 5)
#define SYS_REQ_KERNEL_THREAD_MSG			(SYS_REQ_KERNEL_BASE + 6)

#define SYS_REQ_KERNEL_VIRTUAL_ALLOC		(SYS_REQ_KERNEL_BASE + 20)

#define SYS_REQ_KERNEL_MISC_DRAW_SCREEN		(SYS_REQ_KERNEL_BASE + 90)
#define SYS_REQ_KERNEL_MAX					(SYS_REQ_KERNEL_BASE + 100)

/************************************************************************/
/* LIBC request id and range                                            */
/************************************************************************/
#define FS_REQ_BASE_ID			1
#define SYS_REQ_FILE_BASE		KE_SRV_MAKE_REQ_NUM(FS_REQ_BASE_ID, 0)
#define SYS_REQ_FILE_OPEN		(SYS_REQ_FILE_BASE + 0)
#define SYS_REQ_FILE_READ		(SYS_REQ_FILE_BASE + 1)
#define SYS_REQ_FILE_WRITE		(SYS_REQ_FILE_BASE + 2)
#define SYS_REQ_FILE_CLOSE		(SYS_REQ_FILE_BASE + 3)
#define SYS_REQ_FILE_CHDIR		(SYS_REQ_FILE_BASE + 4)
#define SYS_REQ_FILE_FSTAT		(SYS_REQ_FILE_BASE + 5)
#define SYS_REQ_FILE_FTRUNCATE	(SYS_REQ_FILE_BASE + 6)
#define SYS_REQ_FILE_CREATE		(SYS_REQ_FILE_BASE + 7)
#define SYS_REQ_FILE_READDIR	(SYS_REQ_FILE_BASE + 8)
#define _SYS_REQ_FILE_MAX		9

/************************************************************************/
/* Session and User Manager                                             */
/************************************************************************/
#define SESSION_REQ_BASE_ID					2
#define SESSION_REQ_BASE					KE_SRV_MAKE_REQ_NUM(SESSION_REQ_BASE_ID, 0)
#define SYS_REQ_SESSION_STDIO_READ			(SESSION_REQ_BASE + 0)
#define SYS_REQ_SESSION_STDIO_WRITE			(SESSION_REQ_BASE + 1)

#endif
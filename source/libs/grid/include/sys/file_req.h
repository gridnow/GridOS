#ifndef FILE_SYS_REQ
#define FILE_SYS_REQ

#ifndef _MSC_VER
#include <compiler.h>
#include <types.h>

#else
#include <Windows.h>
#endif

#include "syscall.h"

/************************************************************************/
/* SYSREQ                                                               */
/************************************************************************/

/**
	@brief Request package for file opening
*/
struct sysreq_file_open
{
	/* INPUT */
	struct sysreq_common base;
	xstring	__user name;
	bool			exclusive;		/* 是否是独占打开 */
	unsigned int	file_type;

	/* OUTPUT */
	int				result_code;
	lsize_t			file_size;
};

/**
	@brief Request package for file create
*/
struct sysreq_file_create
{
	/* INPUT */
	struct sysreq_common base;
	xstring 		__user name;

	/* OUTPUT */	
	int				result_code;
};

/**
	@brief Request package for file closing
*/
struct sysreq_file_close
{
	/* INPUT */
	struct sysreq_common base;
	struct __close_file__
	{
		void *		subsystem_object;
		int			fd;
	}file;	

	/* OUTPUT */
	int				result_code;
};

/**
	@brief Request package for file chdir
*/
struct sysreq_file_chdir
{
	/* INPUT */
	struct sysreq_common base;
	xstring 		__user dir_name;

	/* OUTPUT */
	int				result_code;
};

/**
	@brief Request package for file writing/reading
*/
struct sysreq_file_io
{
	/* INPUT */
	struct sysreq_common base;
	
	ke_handle	file;
	uoffset		pos;
	void *		__user buffer;
	size_t		size;
	
	/* OUTPUT */
	lsize_t		current_size;
	size_t		result_size;
};


/**
	@brief Request package for fstat
*/
struct sysreq_file_fstat
{
	/* INPUT */
	struct sysreq_common base;
	
	union
	{
		void *		subsystem_object;
		int			fd;
	}file;

	/* OUTPUT */
	uoffset		size;
	int			result_code;
};

/**
	@brief Request package for ftruncate
*/
struct sysreq_file_ftruncate
{
	/* INPUT */
	struct sysreq_common base;
	
	ke_handle		file;
	void *			__user buffer;
	uoffset			length;

	/* OUTPUT */
	int			result_code;
};
/************************************************************************/
/* INTERFACE                                                            */
/************************************************************************/
struct file;

/**
	@brief Read a file

	@return The bytes system has read, < 0 for error code
 */
ssize_t sys_read(struct file *filp, void *user_buffer, uoffset file_pos, ssize_t n_bytes);

/**
	@brief Write a file
	
	@return The bytes of data been written, < 0 for error code
 */
ssize_t sys_write(ke_handle file, void *user_buffer, uoffset file_pos, ssize_t n_bytes);

#endif

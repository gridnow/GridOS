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
	bool			exclusive;							/* 是否是独占打开 */
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

	/* OUTPUT */
	ke_handle file;
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

/**
	@brief Request package for readdir
*/
struct sysreq_file_readdir
{
	/* INPUT */
	struct sysreq_common base;	
	int start_entry;									/* 读取目录那一块，0 表示开始*/
	ke_handle dir;

	struct dirent_buffer
	{
		unsigned short name_length;
		unsigned short entry_type;
		xchar name[0];

		/* Note that he dirrent_buffer should be accessed in "short" boundary */
	} __user *buffer;									/* 传入一片内存，文件子系统负责填充每项*/	
	int max_size;
	
	/* OUTPUT */
	int next_entry;										/* 为-1表示已经没有内核可以读取了*/
};

#endif

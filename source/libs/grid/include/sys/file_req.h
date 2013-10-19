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
	int			fd;

	int				result_code;

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
	int			fd;
	
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
	
	int			fd;
	uoffset		pos;
	void *		__user buffer;
	size_t		size;
	
	/* OUTPUT */
	size_t		result_size;
	int			result_code;
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
	
	int			fd;
	void *			__user buffer;
	uoffset			length;

	/* OUTPUT */
	int			result_code;
};
/************************************************************************/
/* INTERFACE                                                            */
/************************************************************************/

/**
	@brief Read a file by fd

	@return The bytes system has read, read errno for the reason of failure.
*/
size_t sys_read(int fd, void *user_buffer, uoffset file_pos, ssize_t n_bytes);

/**
	@brief Write a file by fd
	
	@return The bytes system has written, read errno for the reason of failure.
*/
size_t sys_write(int fd, void *user_buffer, uoffset file_pos, ssize_t n_bytes);

/**
	@brief Truncate a file by fd
	
	@return The bytes system has truncated, read errno for the reason of failure.
*/
size_t sys_truncate(unsigned int fd, ssize_t size);

/**
	@brief Open a file by name

	The system will analyze the path in the given name when it tries to open the file.

	@param[in] name open file name
	@param[in] file_type 要打开的文件类型，具体定义在sys/stat.h中

	@return The fd of the file on success, read errno for the reason of failure.
*/
int sys_open(const char *name, unsigned int file_type);

/**
	@brief Create a file by fd
	
	@return The fd of the file on success, read errno for the reason of failure.
*/
int sys_mkfile(const char *name);

/**
	@brief Close a file by fd
	
	@return The result_code of the system request.
*/
int sys_close(int fd);

struct stat *buf;
int sys_fstat(int fd, struct stat *buf);

/**
	@brief 调整文件对象大小

	@param[in] int fd			目标文件对象的文件描述符
	@param[in] ssize_t length	将目标文件对象长度调整为length

	@return	成功返回0，失败则为-1;
*/
int sys_ftruncate(int fd, ssize_t length);

#endif

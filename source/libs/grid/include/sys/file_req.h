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
	bool			exclusive;		/* �Ƿ��Ƕ�ռ�� */
	unsigned int	file_type;

	/* OUTPUT */
	struct __open_file__
	{
		void *		subsystem_object;
		int			fd;
	}file;	

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
	struct __create_file__
	{
		void *		subsystem_object;
		int			fd;
	}file;	

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
	union
	{
		void *		subsystem_object;
		int			fd;
	}file;
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
	file_size	size;
	int			result_code;
};

/**
	@brief Request package for ftruncate
*/
struct sysreq_file_ftruncate
{
	/* INPUT */
	struct sysreq_common base;
	
	union
	{
		void *		subsystem_object;
		int			fd;
	}file;

	void *			__user buffer;
	file_size		length;

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
	@param[in] file_type Ҫ�򿪵��ļ����ͣ����嶨����sys/stat.h��


	@return The fd of the file on success, read errno for the reason of failure.
*/
int sys_open(const char *name, unsigned int file_type);

/**
	@brief Create a file by fd
	
	@return The fd of the file on success, read errno for the reason of failure.
*/
int sys_create(const char *name, int type);

/**
	@brief Close a file by fd
	
	@return The result_code of the system request.
*/
int sys_close(int fd);

struct stat *buf;
int sys_fstat(int fd, struct stat *buf);

/**
	@brief �����ļ������С

	@param[in] int fd			Ŀ���ļ�������ļ�������
	@param[in] ssize_t length	��Ŀ���ļ����󳤶ȵ���Ϊlength

	@return	�ɹ�����0��ʧ����Ϊ-1;
*/
int sys_ftruncate(int fd, ssize_t length);

#endif

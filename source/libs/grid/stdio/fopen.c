/**
	The Grid Core Library
 */

/**
	Posix file
	ZhaoYu,Yaosihai
 */
#include <compiler.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include "file.h"

#define FLAG_LENGTH 3

/*
	用于检查字符串中是否有重复的字符
	返回值为true说明有重复，为false说明无重复
 
	该算法用空间换时间,TODO:有待优化
 */
static bool check_repeat(const char *str)
{
	/* ASCII码最大值为255，该算法利用这个数组空间换取算法效率 */
	unsigned char ascii_array[256] = {0};
	
	while (*str != '\0')
	{
		/* 字符ASCII码对应的数组元素加1 */
		ascii_array[*str]++;
		
		/* 同一字符ASCII码对应的数组元素如果大于等于2，说明有重复 */
		if (ascii_array[*str] >= 2) return true;
		str++;
	}
	
	return false;
}

/*
	@brief 为stdio_file设置具体的flags
 
	r	打开只读文件，该文件必须存在；
	r+	打开可读写文件，该文件必须存在；
	w	打开只写文件，若文件存在则清除文件内容，即文件长度为0，
		若文件不存在则创建文件；
	w+	打开可读写文件，若文件存在则清除文件内容，即文件长度为0，
		若文件不存在则创建文件；
	a	附加方式打开只写文件，若文件存在则写入数据只会追加至文件尾，文件原数据无法修改，
		若文件不存在则创建文件；
	a+	附加方式打开可读写文件，若文件存在则写入数据只会追加至文件尾，文件原数据无法修改，
		若文件不存在则创建文件。
 
	b	二进制流
 
	@return 成功返回true，失败则返回false
 */
static bool set_file_flags(struct stdio_file *file, const char *type)
{
	bool ret = true;
	int	access_modes, status_flags, count;
	
	if (strlen(type) > FLAG_LENGTH) return false;
	
	/* 检查flags中是否有重复设置 */
	if (check_repeat(type)) return false;
	
	/* r/w/a为第一个字符 */
	switch (type[0])
	{
		case 'r':
			access_modes = O_RDONLY;
			status_flags = 0;
			break;
		case 'w':
			access_modes = O_WRONLY;
			status_flags = O_CREAT | O_TRUNC;
			break;
		case 'a':
			access_modes = O_WRONLY;
			status_flags = O_CREAT | O_APPEND;
			break;
			
		default:
			return false;
	}
	
	/* 'b'和'+'位于/r/w/a后，其顺序可不受限制，如/r/w/ab+、/r/w/a+b等效 */
	for (type++; *type != '\0'; type++)
	{
		switch (*type)
		{
			case 'b':
				break;
			case '+':
				access_modes = O_RDWR;
				break;
			default:
				return false;
		}
	}
	
	file->flags = access_modes | status_flags;
	return ret;
}

static bool set_file_fd(struct stdio_file *file, const char *path)
{
	int fd;
	bool ret = true;
	
	// TODO LOCK fd
	
	fd = sys_open(path, S_IFREG);
	
	if (POSIX_INVALID_FD == fd)
	{
		if (file->flags & O_CREAT)
		{
			/* 创建长度为0的文件 */
			fd = sys_mkfile(path);
		}
		else
		{
			/* 不能打开不存在的文件 */
			return false;
		}
	}
	/* 文件存在，需要根据进一步处理 */
	else if (file->flags & O_TRUNC)
	{
		/* 文件长度截短为0 */
		if (-1 == sys_ftruncate(fd, 0)) return false;
	}
	
	/* 为文件对象设置fd */
	file->fd = fd;
	// TODO UNLOCK fd
	
	return ret;
}

static struct stdio_file *open_stream_file(const char *path, const char *type)
{
	struct stdio_file *file;
	
	file = malloc(sizeof(struct stdio_file));
	if (!file)	goto end_exit;
	memset(file, 0, sizeof(struct stdio_file));
	
	/*  */
	if (false == set_file_flags(file, type)) goto err;
	
	/* 文件描述符 */
	if (false == set_file_fd(file, path)) goto err;
	
	stream_file_init_ops(file);
	
	return file;
	
err:
	free(file);
	
	file = NULL;
	
end_exit:
	return file;
}

DLLEXPORT FILE *fopen(const char *path, const char *type)
{
	return (FILE *)open_stream_file(path, type);
}
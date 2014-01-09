/**
	The Grid Core Library
 */

/**
	Posix file
	ZhaoYu,Yaosihai
 */

#ifndef __STREAM_FILE_H__
#define __STREAM_FILE_H__

#include "file.h"
#include "posix.h"

/* We are operating on the file exclusively */
#define LOCK_FILE(F)
#define UNLOCK_FILE(F)

/************************************************************************/
/* Stream file buffer configuration                                     */
/************************************************************************/
#define BUF_SIZE				(1024 * 1024 * 4)
#define BUF_BLOCK_SIZE			(1024 * 256)
#define BUF_BLOCK_NUM			((BUF_SIZE) / (BUF_BLOCK_SIZE))

#define	BUF_BLOCK_VALID_FLAG	0x0001
#define BUF_BLOCK_DIRTY_FLAG	0x0002
#define BUF_BLOCK_BUSY_FLAG		0x0004

#define	BUF_BLOCK_ID(pos)		((pos) / (BUF_BLOCK_SIZE))
#define BUF_BLOCK_OFFSET(pos)	((pos) % (BUF_BLOCK_SIZE))

struct stdio_file;
struct buffer_block
{
	int						valid_size;					/* 有效数据大小，最大不超过size*/
	int						pre_id;
	int						access_count;
	volatile unsigned int	flags;						/* CPU之间等待flags */
	void					*base;
	struct stdio_file		*file;						/* file字段不为NULL说明该buffer_block已被占用 */
};

struct stdio_file
{
	int								flags;	
	unsigned long					lock;
	struct buffer_block				*block;
	void							*private_file;
};

void stream_file_init_ops(struct file *filp);
bool stream_file_buffer_init();

#endif
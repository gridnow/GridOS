/**
	The Grid Core Library
 */

/**
	Stream file
	ZhaoYu,Yaosihai
 */

#include <stdio.h>
#include <stdlib.h>
#include <types.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "sys/file_req.h"
#include "file.h"

static struct buffer_block buffer_pool[BUF_BLOCK_NUM];

/**
	@brief Append a file by fd
 */
static ssize_t append_stream_file(int fd, void *user_buffer, uoffset old_file_size, ssize_t append_bytes)
{
	if (-1 != ftruncate(fd, old_file_size + append_bytes))
	{
		return sys_write(fd, user_buffer, old_file_size, append_bytes);
	}
	else
	{
		return -1;
	}
}

/**
	@brief 将buffer block中的脏数据更新到文件
 
	@return 成功返回true，失败则返回false
 
	@note 本函数的运行需要在锁的返回内，因为buffer block可能随时被抢占
 */
static bool update_dirty_buffer_block(struct stdio_file *file)
{
	bool ret = true;
	int size = -1;
	
	if (!file) return false;
	
	if (file->block->flags & BUF_BLOCK_DIRTY_FLAG)
	{
		/*
		 情况1：O_APPEND模式下，任何写入的数据只能追加到文件尾部
		 情况2：非fO_APPEND模式下，如果file->pos > file->size，写入数据超过文件大小部分追加到文件尾部
		 */
		if (file->flags & O_APPEND || file->pos > file->size)
		{
			size = append_stream_file(file->fd, file->block->base, file->size, file->pos - file->size);
		}
		else
		{
			/* 将buffer block的BUF_BLOCK_OFFSET(file->pos)字节写入file的file->pos-BUF_BLOCK_OFFSET(file->pos)开始处 */
			size = sys_write(file->fd, file->block->base, file->pos - BUF_BLOCK_OFFSET(file->pos), BUF_BLOCK_OFFSET(file->pos));
		}
		size = BUF_BLOCK_OFFSET(file->pos);
		
		/* 只要不是磁盘IO出错都没问题 */
		/*
		 if (errno == EIO)
		 {
		 file->block->flags &= (~BUF_BLOCK_DIRTY_FLAG);
		 ret = false;
		 }*/
	}
	return ret;
}

/**
	@brief 换出buffer block
 
	保证该buffer block能被作为空闲使用，同时如果有脏数据也需要更新到文件
	
	@return 成功返回true，失败则返回false
 */
static bool swap_out_buffer_block(struct buffer_block *block)
{
	bool ret = false;
	
	ret = update_dirty_buffer_block(block->file);
	
	if (true == ret)
	{
		/* 解除原file和该buffer block的关系，但不能影响到原file的其他属性 */
		block->file->block		= NULL;
		block->file				= NULL;
		block->access_count		= 0;
		block->flags			= 0;
		block->valid_size		= 0;
		block->pre_id			= 0;
	}
	
	return ret;
}

/**
	@brief 从缓冲区中找出可被替换为空闲的buffer block
 
	@return 成功返回指向“重获自由的buffer block”的指针，失败则返回NULL
 */
static struct buffer_block *get_rebirth_buffer_block()
{
	int i, mix_access_count, mix_index;
	struct buffer_block *rebirth_buffer_block;
	
	/* 查找访问计数最小的buffer block */
	mix_access_count	= buffer_pool[0].access_count;
	mix_index			= 0;
	for (i = 1; i < BUF_BLOCK_NUM; i++)
	{
		if (buffer_pool[i].access_count < mix_access_count)
		{
			mix_access_count	= buffer_pool[i].access_count;
			mix_index			= i;
		}
	}
	
	/* 保证buffer block的原拥有者file不能操作buffer block */
	LOCK_FILE(buffer_pool[mix_index].file);
	/* buffer block不在忙时 */
	if (!(buffer_pool[mix_index].flags & BUF_BLOCK_BUSY_FLAG))
	{
		/* 换出buffer block */
		i = swap_out_buffer_block(&(buffer_pool[mix_index]));
		if (true == i)
		{
			rebirth_buffer_block = &(buffer_pool[mix_index]);
		}
	}
	UNLOCK_FILE(buffer_pool[mix_index].file);
	
	return rebirth_buffer_block;
}

/**
	@brief 从缓冲区中分配空闲buffer block
 
	@return 成功返回指向空闲buffer block的指针，失败则返回NULL
 */
static struct buffer_block *get_free_buffer_block()
{
	int i, index;
	struct buffer_block *block;
	
	// TODO lock buffer_pool
	for (i = 0; i < BUF_BLOCK_NUM; i++)
	{
		/* buffer_block的file字段为NULL说明该buffer block空闲 */
		if (!buffer_pool[i].file)
		{
			block = &(buffer_pool[i]);
			break;
		}
	}
	// TODO unlock lock buffer_pool
	
	return block;
}

/**
	@brief 为stdio_file分配空闲buffer block
 */
static void get_buffer_block(struct stdio_file *file)
{
	int i, index;
	struct buffer_block *block;
	
start:
	/* 从buffer pool中分配空闲buffer block */
	file->block = get_free_buffer_block();
	if (NULL != file->block) goto end;
	
	/*
	 说明buffer pool中没有空闲buffer block
	 从buffer pool选择一个合适的buffer block作为空闲buffer block换出
	 */
	file->block = get_rebirth_buffer_block();
	if (NULL != file->block) goto end;
	
	goto start;
	
end:
	file->block->file = file;
	file->block->flags |= BUF_BLOCK_BUSY_FLAG;
	file->block->access_count++;
}

static void put_buffer_block(struct stdio_file *file)
{
	file->block->flags &= (~BUF_BLOCK_BUSY_FLAG);	/* 取消BUF_BLOCK_BUSY_FLAG位 */
}


/**
	@brief 为填充buffer block

	@return 成功返回true，失败则返回false
*/
static bool make_valid_data(struct stdio_file *file)
{
	int size;
	bool ret = true;
	int pos;

	/* buffer block满，也就是切换buffer block时检查脏数据更新 */
	if (file->block->pre_id != BUF_BLOCK_ID(file->pos))
	{
		update_dirty_buffer_block(file);
		file->block->flags		&= (~BUF_BLOCK_VALID_FLAG);
	}

	/* Disk to buffer block if buffer is not valid */
	if (!(file->block->flags & BUF_BLOCK_VALID_FLAG))
	{
		/* 
			将file按照BUF_BLOCK_SIZE个数分割，分割后buffer block的变换如下：
			BUF_BLOCK_ID = 0，映射文件0~255
			其中，file->pos = 0表示文件首字节，file->pos = 255表示文件的256字节，及buffer block的末字节

			BUF_BLOCK_ID = 1，映射文件256~511
			……
		*/
		pos = BUF_BLOCK_ID(file->pos) * BUF_BLOCK_SIZE;
		/* 从file的pos开始处，读取BUF_BLOCK_SIZE字节到buffer block中 */
		size = sys_read(file->fd, file->block->base, pos, BUF_BLOCK_SIZE);
		if (size > 0)
		{
			file->block->valid_size	= size;
			file->block->flags		|= BUF_BLOCK_VALID_FLAG;
		}
		else
		{
			ret = false;
		}
	}

	return ret;
}

/**
	@brief 读取数据

	@return 正数表示实际读取的字节数，-1表示出错
*/

static ssize_t stdio_fread(void *ptr, void *buf, ssize_t n_bytes)
{		
	size_t start_pos, tmp_len, i;
	struct stdio_file *file	= (struct stdio_file *)ptr;

	/* 文件只允许写操作 */
	if (file->flags & O_WRONLY)
	{
		set_errno(EPERM);
		return -1;
	}

	/* 文件并无数据 */
	if (0 == file->size)
	{
		return 0;
	}

	LOCK_FILE(file);

	/* 文件对象未分配buffer block时需要分配 */
	if (!file->block)
		get_buffer_block(file);	

	/* 满足规范对type中'+'的限制，保证fread时脏数据被更新 */
	update_dirty_buffer_block(file);

	tmp_len		= i = 0;
	start_pos	= file->pos;

	/* file->pos = (file->size - 1)表示file末字节 */
	while (file->pos < file->size && n_bytes > 0)
	{
		/* make_valid_data失败则循环执行，直到成功 */
		if (make_valid_data(file))
		{
			/* tmp_len不会小于0，因为file->pos小于file->size */
			tmp_len = (file->block->valid_size - BUF_BLOCK_OFFSET(file->pos));
			if (tmp_len > n_bytes)
				tmp_len = n_bytes;
			
			/* 会存在tmp_len为0的情况吗 */


			memcpy(buf + i, file->block->base + BUF_BLOCK_OFFSET(file->pos), tmp_len);
	
			file->block->pre_id = BUF_BLOCK_ID(file->pos);
			n_bytes				-= tmp_len;
			file->pos			+= tmp_len;
			i					+= tmp_len;
		}
		else
		{
			/* seterrno */
			
			goto err;
		}
	}

err:
	put_buffer_block(file);

	UNLOCK_FILE(file);

	return file->pos - start_pos;
}

/**
	@brief 读取数据

	@return 正数表示实际读取的字节数，0表示已到文件结尾，-1表示出错
*/
static ssize_t stdio_fwrite(void *ptr, void *buf, ssize_t n_bytes)
{
	size_t start_pos, tmp_len, i;
	struct stdio_file *file	= (struct stdio_file *)ptr;

	/* 文件只允许读操作 */
	if((file->flags & O_WRONLY) == 0 && (file->flags & O_RDWR) == 0)
	{
		return 0;	
	}

	LOCK_FILE(file);

	/* 文件对象未分配buffer block时需要分配 */
	if (!file->block)
		get_buffer_block(file);

	/* posix标准O_APPEND模式下，stdio_fwrite不会受file->pos影响，始终从file->size处追加数据 */
	if(file->flags & O_APPEND)
	{
		file->pos = file->size;
	}

	tmp_len		= i = 0;
	start_pos	= file->pos;

	while (n_bytes > 0)
	{
		/*
			我们只关心“磁盘IO错误的errno”，如果“没有数据的errno”则认为是file->pos >= file->size时的情况，
			此时将扩展文件大小
		*/
		if(make_valid_data(file) == EIO)
		{
			goto err;
		}

		/* 
			make_valid_data(file)成功
			file->pos >= file->size时都执行下面的代码	
		*/	
		tmp_len = (BUF_BLOCK_SIZE - BUF_BLOCK_OFFSET(file->pos));
		if (tmp_len > n_bytes)
			tmp_len = n_bytes;

		/* user buffer数据覆盖buffer block中的数据 */
		memcpy(file->block->base + BUF_BLOCK_OFFSET(file->pos), buf + i, tmp_len);

		file->block->pre_id = BUF_BLOCK_ID(file->pos);

		n_bytes		-= tmp_len;
		file->pos	+= tmp_len;
		i			+= tmp_len;

		file->block->flags |= BUF_BLOCK_DIRTY_FLAG;
	}
err:
	put_buffer_block(file);

	UNLOCK_FILE(file);
	
	return file->pos - start_pos;
}

/**
	@brief 定位流

	@return 成功返回0，失败则返回-1
*/
static int stdio_fseek(void *ptr, loff_t offset, int whence)
{	
	long tmp_pos;
	int ret = 0;
	struct stdio_file *file	= (struct stdio_file *)ptr;

	LOCK_FILE(file);
	
	if (file->block)
	{
		update_dirty_buffer_block(file);
	}

	switch (whence)
	{
	case SEEK_SET:
		tmp_pos = offset;
		break;

	case SEEK_CUR:
		tmp_pos = file->pos + offset;
		break;

	case SEEK_END:
		tmp_pos = file->size + offset;
		break;

	default:
		ret = -1;
	}

	if(tmp_pos < 0) 
	{
		ret = -1;
		goto err;	
	}

	file->pos = tmp_pos;

err:
	UNLOCK_FILE(file);
	return ret;
}

/**
	@brief 关闭流

	@return 成功返回0，失败则返回EOF
*/
static ssize_t stdio_fclose(void *ptr)
{
	int ret = 0;
	struct stdio_file *file	= (struct stdio_file *)ptr;
	if (!file) return -1;

	LOCK_FILE(file);
	if (file->block)
	{
		swap_out_buffer_block(file->block);
		file->block = NULL;
	}
	sys_close(file->fd);

	UNLOCK_FILE(file);

	free(file);

	return ret;
}


static const struct file_operations stdio_functions = {
	.read		= stdio_fread,
	.write		= stdio_fwrite,
	.seek		= stdio_fseek,
	.close		= stdio_fclose
};

void stream_file_init_ops(struct stdio_file *file)
{
	file->ops = &stdio_functions;
}

bool stream_file_buffer_init()
{
	int	i, j, index, buf_block_size;
	void *pool;
	struct buffer_block *block;
	
	buf_block_size = sizeof(struct buffer_block);
	pool = malloc(BUF_SIZE);
	if (!pool)
		goto err;
	//TODO: init_buffer will use the Virtual Alloc to create buffer pool
	
	for (i = 0, j = 0; i < (BUF_BLOCK_NUM * buf_block_size); i += buf_block_size, j += BUF_BLOCK_SIZE)
	{
		index = i / buf_block_size;
		
		buffer_pool[index].access_count	= 0;
		buffer_pool[index].flags		= 0;
		buffer_pool[index].base			= pool + j;
	}

	return true;
err:
	return false;
}
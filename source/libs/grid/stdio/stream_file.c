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
#include <ddk/debug.h>

#include "sys/file_req.h"
#include "stream_file.h"

//TODO ： 该变量需要加锁
static struct buffer_block buffer_pool[BUF_BLOCK_NUM];

/**
	@brief Append a file by fd
 */
static ssize_t append_stream_file(ke_handle handle, void *user_buffer, lsize_t old_file_size, ssize_t append_bytes)
{
	ssize_t ret;
	
	//TODO, get fd
#if 0
	ret = ftruncate(fd, old_file_size + append_bytes);
	if (ret)
		return ret;
#endif	
	ret = sys_write(handle, user_buffer, 
		old_file_size, BUF_BLOCK_SIZE);
	return ret;
}

/**
	@brief 将buffer block中的脏数据更新到文件
 
	@return 成功返回true，失败则返回false
 
	@note 本函数的运行需要在锁的返回内，因为buffer block可能随时被抢占
 */
static bool update_dirty_buffer_block(struct stdio_file *file)
{
	bool ret = false;
	ssize_t size;
	lsize_t fsize = file_get_from_detail(file)->size;
	uoffset pos = file_get_from_detail(file)->pos;

	if (!(file->block->flags & BUF_BLOCK_DIRTY_FLAG))
	{
		ret = true;
		goto end;
	}
	
	/*
		 情况1：O_APPEND模式下，任何写入的数据只能追加到文件尾部
		 情况2：非fO_APPEND模式下，如果file->pos > file->size，写入数据超过文件大小部分追加到文件尾部
	*/
	if (file->flags & O_APPEND || pos > fsize)
	{
		size = append_stream_file(file_get_from_detail(file)->handle, file->block->base, fsize, pos - fsize);
	}
	else
	{
		/* 将buffer block的BUF_BLOCK_OFFSET(file->pos)字节写入file的file->pos-BUF_BLOCK_OFFSET(file->pos)开始处 */
		size = sys_write(file_get_from_detail(file)->handle, file->block->base, pos - BUF_BLOCK_OFFSET(pos), 
			BUF_BLOCK_SIZE);
	}

	if (size < 0)
		goto end;
	
	/* 只要不是磁盘IO出错都没问题 */
	file->block->flags &= (~BUF_BLOCK_DIRTY_FLAG);
	ret = true;
end:
	if (ret != true)
	{
		/* 写入失败，告知系统延迟写! */
		//TODO
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
	bool ret;
	
	ret = update_dirty_buffer_block(block->file);

	/* 解除原file和该buffer block的关系，但不能影响到原file的其他属性 */
	block->file->block		= NULL;
	block->file				= NULL;
	block->access_count		= 0;
	block->flags			= 0;
	block->valid_size		= 0;
	block->pre_id			= 0;
	
	return ret;
}

/**
	@brief 从缓冲区中找出可被替换为空闲的buffer block
 
	@return 成功返回指向“重获自由的buffer block”的指针，失败则返回NULL
 */
static struct buffer_block *get_rebirth_buffer_block()
{
	int i, mix_access_count, mix_index;
	struct buffer_block *rebirth_buffer_block = NULL;
	
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
	int i;
	struct buffer_block *block = NULL;
	
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
	do
	{
		/* 从buffer pool中分配空闲buffer block */
		file->block = get_free_buffer_block();
		if (NULL != file->block) 
			break;
				
		file->block = get_rebirth_buffer_block();
		if (NULL != file->block) 
			break;
	} while (1);
	
	file->block->file = file;
	file->block->flags |= BUF_BLOCK_BUSY_FLAG;
	file->block->access_count++;
}

static void put_buffer_block(struct stdio_file *file)
{
	file->block->flags &= (~BUF_BLOCK_BUSY_FLAG);	/* 取消BUF_BLOCK_BUSY_FLAG位 */
}

static bool make_valid_data(struct stdio_file *file)
{
	ssize_t size;
	bool ret = true;
	uoffset pos;
	uoffset fpos = file_get_from_detail(file)->pos;

	/* buffer block满，也就是切换buffer block时检查脏数据更新 */
	if (file->block->pre_id != BUF_BLOCK_ID(fpos))
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
		pos = BUF_BLOCK_ID(fpos) * BUF_BLOCK_SIZE;

		/* 从file的pos开始处，读取BUF_BLOCK_SIZE字节到buffer block中 */
		size = sys_read(file_get_from_detail(file), file->block->base, pos, BUF_BLOCK_SIZE);
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

static ssize_t stdio_fread(struct file *filp, void *buf, ssize_t n_bytes)
{		
	ssize_t ret = -1;
	size_t start_pos, tmp_len, i;
	struct stdio_file *file	= file_get_detail(filp);

	if (file->flags & O_WRONLY)
	{
		set_errno(EPERM);
		goto end;
	}		
	if (0 == filp->size)	
	{
		ret = 0;
		goto end;	
	}

	LOCK_FILE(file);

	if (!file->block)
		get_buffer_block(file);	
	tmp_len		= i = 0;
	start_pos	= filp->pos;

	/* file->pos = (file->size - 1)表示file末字节 */
	while (filp->pos < filp->size && n_bytes > 0)
	{		
		if (make_valid_data(file) == false)
			goto end_grab;
		//printf("pos = %d, size = %d, valid = %d.\n", (int)filp->pos, (int)filp->size, file->block->valid_size);

		/* tmp_len不会<=0，因为file->pos小于file->size */
		tmp_len = (file->block->valid_size - BUF_BLOCK_OFFSET(filp->pos));
		if (tmp_len > n_bytes)
			tmp_len = n_bytes;
		memcpy(buf + i, file->block->base + BUF_BLOCK_OFFSET(filp->pos), tmp_len);

		file->block->pre_id = BUF_BLOCK_ID(filp->pos);
		n_bytes				-= tmp_len;
		filp->pos			+= tmp_len;
		i					+= tmp_len;
	}

	ret = filp->pos - start_pos;

end_grab:
	put_buffer_block(file);

	UNLOCK_FILE(file);
end:
	return ret;
}

static ssize_t stdio_fwrite(struct file *filp, void *buf, ssize_t n_bytes)
{
	ssize_t ret = -1;

	size_t start_pos, tmp_len, i;
	struct stdio_file *file	= file_get_detail(filp);
	
	/* 文件只允许读操作 */
	if((file->flags & O_WRONLY) == 0 && (file->flags & O_RDWR) == 0)
	{
		ret = 0;
		goto end;			
	}

	LOCK_FILE(file);

	if (!file->block)
		get_buffer_block(file);

	/* posix标准O_APPEND模式下，stdio_fwrite不会受file->pos影响，始终从file->size处追加数据 */
	if(file->flags & O_APPEND)
	{
		filp->pos = filp->size;
	}

	tmp_len		= i = 0;
	start_pos	= filp->pos;

	while (n_bytes > 0)
	{
		/*
			我们只关心“磁盘IO错误的errno”，如果“没有数据的errno”则认为是file->pos >= file->size时的情况，
			此时将扩展文件大小
		*/
		if(make_valid_data(file) == false && get_errno() == EIO)
		{
			goto end_grab;
		}

		/* 
			make_valid_data(file)成功
			file->pos >= file->size时都执行下面的代码	
		*/	
		tmp_len = (BUF_BLOCK_SIZE - BUF_BLOCK_OFFSET(filp->pos));
		if (tmp_len > n_bytes)
			tmp_len = n_bytes;

		/* user buffer数据覆盖buffer block中的数据 */
		memcpy(file->block->base + BUF_BLOCK_OFFSET(filp->pos), buf + i, tmp_len);

		file->block->pre_id = BUF_BLOCK_ID(filp->pos);

		n_bytes		-= tmp_len;
		filp->pos	+= tmp_len;
		i			+= tmp_len;

		file->block->flags |= BUF_BLOCK_DIRTY_FLAG;
	}
	ret = filp->pos - start_pos;

end_grab:
	put_buffer_block(file);

	UNLOCK_FILE(file);
end:
	return ret;	
}

static int stdio_fseek(struct file *filp, loff_t offset, int whence)
{	
	uoffset tmp_pos;
	int ret = 0;
	struct stdio_file *file;

	file = file_get_detail(filp);
	
	LOCK_FILE(file);
	
	if (file->block)
		update_dirty_buffer_block(file);

	switch (whence)
	{
	case SEEK_SET:
		tmp_pos = offset;
		break;

	case SEEK_CUR:
		tmp_pos = filp->pos + offset;
		break;

	case SEEK_END:
		tmp_pos = filp->size + offset;
		break;

	default:
		tmp_pos = POSIX_FAILED;
		set_errno(EINVAL);
	}

	if(tmp_pos < 0) 
	{
		ret = -1;
		goto err;	
	}

	filp->pos = tmp_pos;

err:
	UNLOCK_FILE(file);
	return ret;
}

static int stdio_fclose(struct file *filp)
{
	int ret = 0;
	struct stdio_file *file;
	
	file = file_get_detail(filp);

	/* Private rollback */
	LOCK_FILE(file);
	if (file->block)
	{
		swap_out_buffer_block(file->block);
		file->block = NULL;
	}
	UNLOCK_FILE(file);

	filp_delete(filp);
	
	return ret;
}

static const struct file_operations stdio_functions = {
	.read		= stdio_fread,
	.write		= stdio_fwrite,
	.seek		= stdio_fseek,
	.close		= stdio_fclose
};

void stream_file_init_ops(struct file *filp)
{
	filp->ops = &stdio_functions;
}

bool stream_file_buffer_init()
{
	int	i, j, index, buf_block_size;
	void *pool;
		
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

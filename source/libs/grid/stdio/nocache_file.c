

#include <stdio.h>
#include <stdlib.h>
#include <types.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <ddk/debug.h>

#include "sys/file_req.h"
#include "nocache_file.h"
#include "stream_file.h"

static ssize_t nocache_fwrite(struct file *filp, void *buf, ssize_t n_bytes)
{
	ssize_t ret = -1;

	struct stdio_file *file	= file_get_detail(filp);

	/* 文件只允许读操作 */
	if((file->flags & O_WRONLY) == 0 && (file->flags & O_RDWR) == 0)
	{
		ret = 0;
		goto end;			
	}
	
	/* posix标准O_APPEND模式下，stdio_fwrite不会受file->pos影响，始终从file->size处追加数据 */
	if(file->flags & O_APPEND)
	{
		filp->pos = filp->size;
	}

	/* write to kernel fss */
	ret = sys_write(filp->handle, buf, filp->pos, n_bytes);

	/* TODO 修改下次写入的位置  系统调用返回的字节数不对 ? */
	filp->pos += n_bytes;
	
end:
	return ret;
}


static ssize_t nocache_fread(struct file *filp, void *buf, ssize_t n_bytes)
{
	ssize_t ret = -1;

	struct stdio_file *file	= file_get_detail(filp);
	
	if (file->flags & O_WRONLY)
	{
		ret = 0;
		goto end;
	}		
	if (0 == filp->size)	
	{
		ret = 0;
		goto end;	
	}

	ret = sys_read(filp, buf, filp->pos, n_bytes);

	/* 记录下次读取的位置 */
	filp->pos += ret;
	
end:
	return ret;
}

static int nocache_fseek(struct file *filp, loff_t offset, int whence)
{
	uoffset tmp_pos;
	int ret = 0;

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
	return ret;
}

static int nocache_fclose(struct file *filp)
{
	filp_delete(filp);
	
	return 0;
}

static const struct file_operations nocache_file_ops = {
	.read		= nocache_fread,
	.write		= nocache_fwrite,
	.seek		= nocache_fseek,
	.close		= nocache_fclose
};


/*
	@brief 
		init nocache file operation
	@return
		void
*/
void nocache_file_init_ops(struct file *filp)
{
	filp->ops = &nocache_file_ops;
}






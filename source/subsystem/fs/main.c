/**
	Filesystem framework
	Copyright(c) 2003-2013 Sihai(yaosihai@yeah.net).
	All rights reserved.
*/

#include <kernel/ke_memory.h>
#include <kernel/ke_srv.h>

#include <ddk/compiler.h>
#include <ddk/debug.h>
#include <errno.h>

#include <vfs.h>
#include <node.h>
#include <cache.h>

#include "sys/file_req.h"

static struct fss_vfs_info fss;


/************************************************************************/
/* Volumn                                                               */
/************************************************************************/
struct fss_volumn *fss_volumn_create_simple()
{
	struct fss_volumn *v = NULL;
	
	v = km_valloc(sizeof(*v));
	if (v)
	{
		memset(v, 0, sizeof(*v));
		ke_rwlock_write_lock(&fss.vol_list_lock);
		list_add_tail(&v->list, &fss.vol_list);
		ke_rwlock_write_unlock(&fss.vol_list_lock);
	}
	
	return v;
}

struct fss_volumn *fss_volumn_search(xstring id)
{
	struct fss_volumn *t;
	int found = 0;

	ke_rwlock_read_lock(&fss.vol_list_lock);
	list_for_each_entry(t, &fss.vol_list, list)
	{
		if (!FSS_STRCMP(id, t->volumn_id))
		{
			found = 1;
			break;
		}
	}
	ke_rwlock_read_unlock(&fss.vol_list_lock);

	if (found)
		return t;
	return NULL;
}

DLLEXPORT void fss_vfs_register(struct fss_vfs_driver *driver)
{
	void *private;
	struct fss_volumn *v = NULL;

	//TODO 支持更多
	printk("注册文件系统 %s...\n", driver->name);
	list_add_tail(&driver->list, &fss.drv_list);
	
	/* Probe with volumn */
	v = fss_volumn_create_simple(/*TODO: set volumn information before probe*/);
	if (v == NULL)
		goto err;
	
	private = driver->ops->fMount(NULL);
	if (private == NULL)
		goto err;
	if (fss_tree_init(v, driver, private) == false)
		goto err;
	printk("注册ok。\n");
	return;
	
err:
	if (v)
	{
		//TODO:free volumn
	}
	if (private)
	{
		//TODO:Close the root file directory
	}
	
	return;
}

/************************************************************************/
/* File                                                                 */
/************************************************************************/
ssize_t fss_dbd_make_valid(struct fss_file * who, struct dbd * which)
{
	unsigned long size = FSS_CACHE_DB_SIZE;
	int ret = -EBADF;
	
	FSS_DBD_LOCK(which);

	if (which->flags & DB_VALID_DATA)
	{
		ret = which->valid_size;
		goto end;
	}

	/* Just an entry, not really opened by the file system driver */
	if (who->private == NULL)
		goto end;	
	ret = who->volumn->drv->ops->fRead(who->private, which->block_id,
									   which->buffer, &size);
	which->valid_size = size;
	which->flags |= DB_VALID_DATA;

end:
	FSS_DBD_UNLOCK(which);
	return ret;
}

struct fss_file *fss_open(struct fss_file *current_dir, char *name)
{
	struct fss_file *f;

	/* 搜索文件将增加文件的引用计数 */
	f = fss_loop_file(current_dir, name, NULL, NULL);
	if (!f)
		goto err0;
	
	//if (!f->private)
	{
		f->private = f->volumn->drv->ops->fOpen(f->parent->private, f->name);
		if (f->private == NULL)
		{
			//TODO: Close the vfs
			TODO("关闭僵尸文件");
		}
	}
	return f;
err0:
	return NULL;
}

void fss_close(struct fss_file *who)
{
	//TODO
}

ssize_t fss_read(struct fss_file * who, unsigned long block, void *buffer)
{
	ssize_t ret = -ENOMEM;
	struct dbd * which = NULL;

	/* Acquire the dbd by offset */
	which = fss_dbd_get(who, block);
	if (unlikely(!which)) 
		goto end;

	/* Fill the dbd with valid data from disk */	
	ret = fss_dbd_make_valid(who, which);
	if (ret < 0)
		goto end;
	
	/* Read */
	memcpy(buffer, which->buffer, FSS_CACHE_DB_SIZE);
	ret = which->valid_size;
	
end:
	if (which)		
		fss_dbd_put(which);
	return ret;
}

lsize_t fss_get_size(struct fss_file *who)
{
	//TODO
	return 1024*1024;
}

/************************************************************************/
/* User system call handler                                             */
/************************************************************************/
static bool check_user_buffer(void *buf, size_t size, int write)
{
	//TODO: Call kernel to validate the buffer 
	return true;
}

static ke_handle req_open(struct sysreq_file_open *req)
{
	ke_handle h;
	struct fss_file *filp, *current_dir = NULL;
	
	//TODO: to support current path
	filp = fss_open(current_dir, req->name);
	if (!filp)
		goto err;
	
	h = ke_handle_create(filp);
	if (h == KE_INVALID_HANDLE)
		goto err;

	req->file_size = fss_get_size(filp);
	
	return h;
	
err:
	if (filp)
		fss_close(filp);
	return KE_INVALID_HANDLE;
}

static ssize_t req_read(struct sysreq_file_io *req)
{
	ssize_t ret = -1;
	struct fss_file *file = NULL;
	unsigned long block = req->pos / FSS_CACHE_DB_SIZE;
	void *buf = req->buffer;

	/* The user buffer can be written? */
	if (req->size != FSS_CACHE_DB_SIZE)
		goto end;
	if (check_user_buffer(buf, req->size, 1) == false)
		goto end;
	file = ke_handle_translate(req->file);
	if (!file)
		goto end;

	ret = fss_read(file, block, buf);
	if (ret < 0)
		goto end;

	req->current_size = ret;
	req->result_size = ret;

end:
	if (file)
		ke_handle_put(req->file, file);
	return ret;
}


/*
	readdir:目录读取的系统调用
	返回目录下文件名,索引号,文件块大小
	文件权限,创建时间
	@return:实际读取文件数目
*/
size_t req_readdir(struct sysreq_file_readdir *req)
{
	struct fss_file *file = NULL, *child_file;
	struct dirent_buffer *entry;
	int rd_count = 0, cpy_count = 0;
	int name_len, cpy_len = 0;
	int err;
	
	/* 请求 entry 是否为0 ? */
	if (!req->max_size)
	{
		err = -EINVAL;
		goto err;
	}

	file = ke_handle_translate(req->dir);	
	/* 文件不存在或者文件不为目录? */
	if ((!file) || (file->type != FSS_FILE_TYPE_DIR))
	{
		err = -ENOTDIR;
		goto err;
	}
	
	/* 
		读取目录下文件到req readdir_entry中.
		在打开的时候,已经将目录下所有的文件都从
		硬盘读入内存,并通过file->brother链接到dir
		->child_head内.
	*/

	list_for_each_entry(child_file, &(file->t.dir.child_head), brother)
	{
		/* 寻找这次读取文件的起点 */
		if (rd_count < req->start_entry)
		{
			rd_count++;
			continue;
		}

		/* 是否还能往buffer中继续写? */
		if (cpy_len + sizeof(struct dirent_buffer) > req->max_size)
			break;
		
		/* 
			写入用户buffer内 
			能copy 文件名长度
		*/
		name_len = FSS_STRLEN(child_file->name);
		name_len = (name_len > (req->max_size - cpy_len - sizeof(struct dirent_buffer))) ?\
					(req->max_size - cpy_len - sizeof(struct dirent_buffer)) :\
					name_len;
		
		entry = (void *)req->buffer + cpy_len;
		entry->entry_type  = child_file->type;
		entry->name_length = name_len;
		FSS_STRNCPY(entry->name, child_file->name, name_len);

		cpy_len += sizeof(struct dirent_buffer) + entry->name_length;
		++cpy_count;
	}

	/* 
		记录下次读取目录下文件的起点 
	*/
	req->next_entry = req->start_entry + rd_count + cpy_count;
	
	err = cpy_len;
err:
	if (file)
		ke_handle_put(req->dir, file);
	return err;
}


/* 处理函数列表，注意必须和SYS_REQ_FILE_xxx的编号一致 */
static void * interfaces[_SYS_REQ_FILE_MAX];

static unsigned long kernel_srv(unsigned long req_id, void *req)
{
	unsigned long (*func)(void * req) = interfaces[req_id];
	return func(req);
}

const static struct ke_srv_info ke_srv_fss = {
	.name = "FSS服务",
	.service_id_base = SYS_REQ_FILE_BASE,
	.request_enqueue = kernel_srv,
};

void fss_main()
{
	int fs_call_num;
	
	INIT_LIST_HEAD(&fss.vol_list);
	INIT_LIST_HEAD(&fss.drv_list);
	ke_rwlock_init(&fss.vol_list_lock);

	fss_db_init();
	fss_map_init();
	
	/* 初始化fss层系统调用函数 */
	for(fs_call_num = 0; fs_call_num < _SYS_REQ_FILE_MAX; ++fs_call_num)
		interfaces[fs_call_num] = ke_srv_null_sysxcal;
	
	interfaces[SYS_REQ_FILE_OPEN - SYS_REQ_FILE_BASE]    = req_open;
	interfaces[SYS_REQ_FILE_READ - SYS_REQ_FILE_BASE]    = req_read;
	interfaces[SYS_REQ_FILE_READDIR - SYS_REQ_FILE_BASE] = req_readdir;
	
	ke_srv_register(&ke_srv_fss);
}



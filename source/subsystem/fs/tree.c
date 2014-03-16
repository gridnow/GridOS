/**
	Filesystem framework
	Copyright(c) 2003-2013 Sihai(yaosihai@yeah.net).
	All rights reserved.
*/

#include <kernel/ke_memory.h>
#include <kernel/ke_lock.h>
#include <ddk/debug.h>

#include <errno.h>

#include <vfs.h>
#include <node.h>
#include <cache.h>

static void tree_thread(struct fss_volumn *v)
{
}


/**
	@brief Volumn of Unix mounting point

	Get the unix root

	@return The volumn descriptor of Unix mounting point
*/
struct fss_volumn *get_unix()
{
	//TODO 
	return fss_volumn_search("0:");
}


/**
	@brief 导入目录下的文件

	如果目录是完整的则根本不导入，如果不是完整的，那么导入时本函数将判断是否重复，因此效率要差一些。

	@return
		> 0 导入了多少个文件，< 0错误码
*/
static int import_file(struct fss_file *current_path)
{
	struct fss_volumn * volumn;
	int ret = -ENOTDIR;
	unsigned long size;
	struct fss_state_info info;
	struct fss_directory_entry *entries = NULL, *p;

	if (current_path->type != FSS_FILE_TYPE_DIR)
		goto end;
	ret = 0;
	
	if (current_path->t.dir.tree_flags & FSS_FILE_TREE_COMPLETION)
		goto end;
	if (!current_path->private/*Not open before*/)
	{
		ret = -EIO;
		current_path->private = current_path->volumn->drv->ops->fOpen(current_path->parent->private, current_path->name);
		if (!current_path->private)
			goto end;
	}

	ret = -EIO;
	volumn = current_path->volumn;
	if (volumn->drv->ops->fStat(current_path->private, &info))
		goto end;

	ret = -ENOMEM;
	size = info.size;
	entries = km_valloc(size);
	if (!entries)
		goto end;

	ret = volumn->drv->ops->fReadDir(current_path->private, entries, size);
	if (ret < 0)
		goto end;

	size = ret;
	for (p = entries; size != 0; p = (void*)p + p->record_len)	
	{
		fss_file_type type = FSS_FILE_TYPE_NORMAL;
		struct fss_file *f;

		if (p->type == 4/*DT_DIR*/)
			type = FSS_FILE_TYPE_DIR;
		f = fss_file_new(current_path, NULL/*Not open, error fd*/, p->name, type);
		if (!f)
		{
			ret = -ENOMEM;
			goto end;
		}

		size -= p->record_len;
	}

	current_path->t.dir.tree_flags |= FSS_FILE_TREE_COMPLETION;
	
end:
	if (entries)
		km_vfree(entries);
	return ret;
}

int fss_tree_make_full(struct fss_file *current_path)
{
	return import_file(current_path);
}

static struct fss_file *get_file_by_name(struct list_head *node, struct fss_file *current_path, xstring file_name, int iStart, int iEnd)
{
	struct fss_file *file = NULL;

	/* 搜索时目录不能被修改 */
	ke_rwlock_read_lock(&current_path->t.dir.dir_lock);

	/* . 表示当前目录, .. 表示上级目录 */
	if (!FSS_STRCMP(file_name + iStart, "."))
	{
		file = current_path;
		goto got;
	}
	if (!FSS_STRCMP(file_name + iStart, ".."))
	{
		file = current_path->parent;
		goto got;
	}

	/* Loop in current path, TODO: using hash to speed up this procedure */
	list_for_each(node, &current_path->t.dir.child_head)
	{
		file = list_entry(node, struct fss_file, brother);
		
		/* 查看得到的file是否是需要的 */
		if (!FSS_STRNCMP(FSS_FILE_GET_NAME(file), file_name + iStart, iEnd - iStart) && FSS_STRLEN(FSS_FILE_GET_NAME(file)) == iEnd - iStart)
			break;
		else
			file = NULL;	/* 如果不是需要的file，返回NULL */
	}

	if (likely(file))
	{
got:
		FSS_FILE_INC_REF_COUNTER(file);		
	}

	ke_rwlock_read_unlock(&current_path->t.dir.dir_lock);

	return file;
}

/**
	@brief find the entry in the dir
*/
static struct fss_file *lockup_file_in_this_dir(struct fss_file *current_path, xstring file_name, int iStart, int iEnd)
{
	struct fss_file * file = NULL;
	struct list_head * tmp = NULL;
	int looped = 0;
	
//	printf("Finding file name %s(current %s) ,start %d,end %d\n", file_name, current_path->name, iStart, iEnd);
	
loop_again:
	file = get_file_by_name(tmp, current_path, file_name, iStart, iEnd);
	if (likely(file)) goto end;

	/* 进入临界区再搜, 这个过程搜不到就要导入磁盘目录，速度较慢 */
	ke_critical_enter(&current_path->t.dir.dir_sleep_lock);
	file = get_file_by_name(tmp, current_path, file_name, iStart, iEnd);
	if (likely(file)) goto unlock_end;

	/* Refill the current path */
	if (!looped && list_empty(&current_path->t.dir.child_head)/*如果是空目录*/)
	{
		if (import_file(current_path) < 0)
			goto unlock_end;
		
		/* Try again */
		looped = 1;
		ke_critical_leave(&current_path->t.dir.dir_sleep_lock);
		goto loop_again;
	}

unlock_end:
	ke_critical_leave(&current_path->t.dir.dir_sleep_lock);
end:
	return file;
}

/**
	@brief 遍历文件Tree,并根据不同的情况调用处理函数。

	@return
		如果有action, action 的返回就是本函数的返回，
		如果没有ACTION，返回当前搜索所在的目录	
*/
struct fss_file * fss_loop_file(struct fss_file * current_path, xstring path_string, void * context, fss_find_action action)
{
	/*
		There are 2 possible formats in path_string:
		1(full path): \0000:\dir\subdir1\ or(0:)
		2(relative path): dir\subdir1\
	*/		
	xchar interval[FSS_VOLUMN_LABEL_CHECK_LEN];
	struct fss_file * cur_dir = current_path, * tmp;
	int i, j;

	/* 
		Is a UNIX like path? 
		Make sure we are accessing the UNIX like monuting tree.
		if so goto root mounting point.
	*/
	if (path_string[0] == '/')
	{
		struct fss_volumn	*fs;

		/*
			看看是否是/x:/abc/cde/ /x;/abc/cde/ 这样的带有特定盘符的路径
			使用常规绝对路径，因为带有盘符.
			: 和 ; 在这种情况下是等价的，主要是处理ALSA它访问路径是，ALSA LIB要将有':'的路径分割成两个，因此不能使用/0:这样的写法
		*/
		for (i = 1/*跳过 '/' */; i < 1 + FSS_VOLUMN_LABEL_CHECK_LEN; i++)
		{
			if (path_string[i] == '\0' || path_string[i] == '/' || path_string[i] == '\\')
				break;
			if (path_string[i] == ':' || path_string[i] == ';')
				break;
		}

		/* 找完了？ 表示没有遇到特殊符号 */
		if (path_string[i] == 0)
			goto must_be_unix;
		/* 没有找完，可能有特殊符号 */
		if ((path_string[i] == ':' || path_string[i] == ';') &&
			(path_string[i+1] == 0 || path_string[i + 1] == '/' || path_string[i + 1] == '\\'))
			goto loop_file_l2;

		/*
			那么，确实是UNIX根
		*/
must_be_unix:
		fs = get_unix();
		if (fs == NULL)
			goto error;
		i = 1/*去掉 '/' */;
		cur_dir = fs->root_dir;				
		goto loop_file_l1;
	}

	/* 
		搜索特殊的符号，以确定路径的类型
	*/
	for (i = 0; i < FSS_VOLUMN_LABEL_CHECK_LEN; i++)
	{
		if (path_string[i] == '\0' || path_string[i] == '/' || path_string[i] == '\\')
			break;
		if (path_string[i] == ':' || path_string[i] == ';')
			break;
	}

	/* 
		绝对路径吗？
	*/
loop_file_l2:
	if ((path_string[i] == ':' || path_string[i]==';') &&
		(path_string[i + 1] == 0 || path_string[i + 1] == '/' || path_string[i + 1] == '\\'))
	{
		struct fss_volumn	*fs;
		int k ;

		/*
			绝对路径，搜索逻辑分区
		*/
		for (j = 0, k = 0; j <= i/*xxxx:,may have / before volumn name.*/; j++)
		{
			if (path_string[j] == '/' || path_string[j] == '\\')
				continue;
			interval[k] = path_string[j];						//copy drive id xxxx:
			k++;		
		}
		if (interval[k - 1] == ';') interval[k - 1] = ':';		//如果是变态的';'，那么使用':'吧；
		interval[k] = '\0';										//now in "xxxx:" form

		fs = fss_volumn_search(interval);

		if (fs == NULL)											//can not find the drive
			goto error;
		cur_dir = fs->root_dir;

		i++;
																//i points to ':',but we do not like it
		/*
		if(path_string[i] == '\0')								//do not still has string?
			goto just_have_dir;
		*/
	}
	else 
	{

		/* 
			找完了，应该是相对路径
		*/
		j = i = 0;												//resolve from the head of the string
	}

	/* 
		都不是？那么cur_dir没有找到，因此是错误的路径格式
	*/
loop_file_l1:
	if (cur_dir == NULL)										//没有当前路径可操作；
		goto error;

	/*
		目前，找到了挂载点，不论是UNIX的还是非UNIX的，每个分区
		都有一颗目录树，现在都找到了对应的上级目录。

		开始在该目录下解析传入的路径.
		
		possible string:

		\dir\dir2
		dir\dir2\
		dir\dir2
		dir
	*/
	while (path_string[i] != '\0')
	{		
		//ok,an usable string.copy the whole string untile we encounter a seprator or 0
		//We meet these,we call the tree to search the node
		//j points to the start of string,i points to the end
		j = i;
		while ((path_string[i] != '\0') &&
			(path_string[i] != '\\') && 
			(path_string[i] != '/'))
			i++;
		
		/*一些小符号可以直接跳过.*/
		if (j == i)
		{
			i++;				
			continue;
		}

		/*Look up*/
		tmp = cur_dir;

		cur_dir = lockup_file_in_this_dir(cur_dir, path_string, j, i);

		if (cur_dir == NULL)									//查找失败，调用用户处理函数
		{
			if (action == NULL)
			{
				goto error;
			}
			cur_dir = (void*)action(tmp, LOOP_NOT_FOUND, path_string, j, i, context);
			if (cur_dir == NULL)
			{				
				goto error;
			}
		}
		else										//找到节点，那么调用特有的处理函数；
		{
			if (action != NULL)	
			{
				cur_dir = (void*)action(cur_dir, LOOP_FOUND, path_string, j, i, context);
				if (cur_dir == NULL)
				{	
					goto error;
				}
			}
		}
	}

	if(action)
		return (void*)action(cur_dir, LOOP_FILE_END, NULL, 0, 0, context);	
	else
		return cur_dir;

error:
	return NULL;
}

struct fss_file *fss_file_new(struct fss_file *father, void *private, const char *name, fss_file_type type)
{
	struct fss_file *file;
	char *file_name;
	
	file		= km_valloc(sizeof(*file));
	file_name	= km_valloc(FSS_STRLEN(name) + FSS_NAME_END_SIZE);
	
	if (!file) goto err;
	if (!file_name) goto err;
	
	memset(file, 0, sizeof(*file));
	file->name = file_name;
	FSS_STRCPY(file->name, name);
	file->type = type;	
	file->private = private;
	fss_mltt_init(file);
	
	INIT_LIST_HEAD(&file->notify_list);
	ke_spin_init(&file->notify_lock);
	
	/* 目录特有的数据 */
	if (type == FSS_FILE_TYPE_DIR)
	{
		ke_rwlock_init(&file->t.dir.dir_lock);
		ke_critical_init(&file->t.dir.dir_sleep_lock);
		INIT_LIST_HEAD(&file->t.dir.child_head);
	}

	/* Only root dir has no father */
	if (likely(father))
	{
		ke_rwlock_write_lock(&father->t.dir.dir_lock);
		list_add_tail(&file->brother, &father->t.dir.child_head);
		ke_rwlock_write_unlock(&father->t.dir.dir_lock);

		/* Root's volumn is set in fss_tree_init */
		file->volumn = father->volumn;
		file->parent = father;
	}
	
	return file;
	
err:
	if (file)
		km_vfree(file);
	if (file_name)
		km_vfree(file_name);
	return NULL;
}


bool fss_tree_init(struct fss_volumn *v, struct fss_vfs_driver *drv, void *root_private)
{
	FSS_STRCPY(v->volumn_id, "0:"/*TODO: to add more volumn */);
	
	/* Create the root file with given private from file system */
	v->root_dir = fss_file_new(NULL, root_private, "根目录", FSS_FILE_TYPE_DIR);
	if (!v->root_dir)
		goto err;
	v->root_dir->volumn = v;

	v->drv = drv;

	/* Root dir is opened */
	ke_atomic_inc(&v->using_count);

	/* Create a thread to maintain the tree on the volumn */
	kt_create_kernel(tree_thread, v);

	return true;
	
err:
	return false;
}

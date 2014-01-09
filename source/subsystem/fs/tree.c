/**
	Filesystem framework
	Copyright(c) 2003-2013 Sihai(yaosihai@yeah.net).
	All rights reserved.
*/

#include <kernel/ke_memory.h>
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
	@brief ����Ŀ¼�µ��ļ�

	���Ŀ¼������������������룬������������ģ���ô����ʱ���������ж��Ƿ��ظ������Ч��Ҫ��һЩ��

	@return
		> 0 �����˶��ٸ��ļ���< 0������
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
	if (current_path->private == (void*)-1/*Not open before*/)
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
		f = fss_file_new(current_path, (void*)-1/*Not open, error fd*/, p->name, type);
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

static struct fss_file *get_file_by_name(struct list_head *node, struct fss_file *current_path, xstring file_name, int iStart, int iEnd)
{
	struct fss_file *file = NULL;

	/* ����ʱĿ¼���ܱ��޸� */
	ke_rwlock_read_lock(&current_path->t.dir.dir_lock);

	/* . ��ʾ��ǰĿ¼, .. ��ʾ�ϼ�Ŀ¼ */
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

		/* �鿴�õ���file�Ƿ�����Ҫ�� */
		if (!FSS_STRNCMP(FSS_FILE_GET_NAME(file), file_name + iStart, iEnd - iStart) && FSS_STRLEN(FSS_FILE_GET_NAME(file)) == iEnd - iStart)
			break;
		else
			file = NULL;	/* ���������Ҫ��file������NULL */
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

	/* �����ٽ�������, ��������Ѳ�����Ҫ�������Ŀ¼���ٶȽ��� */
	ke_critical_enter(&current_path->t.dir.dir_sleep_lock);
	file = get_file_by_name(tmp, current_path, file_name, iStart, iEnd);
	if (likely(file)) goto unlock_end;

	/* Refill the current path */
	if (!looped && list_empty(&current_path->t.dir.child_head)/*����ǿ�Ŀ¼*/)
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
	@brief �����ļ�Tree,�����ݲ�ͬ��������ô�������

	@return
		�����action, action �ķ��ؾ��Ǳ������ķ��أ�
		���û��ACTION�����ص�ǰ�������ڵ�Ŀ¼	
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
			�����Ƿ���/x:/abc/cde/ /x;/abc/cde/ �����Ĵ����ض��̷���·��
			ʹ�ó������·������Ϊ�����̷�.
			: �� ; ������������ǵȼ۵ģ���Ҫ�Ǵ���ALSA������·���ǣ�ALSA LIBҪ����':'��·���ָ����������˲���ʹ��/0:������д��
		*/
		for (i = 1/*���� '/' */; i < 1 + FSS_VOLUMN_LABEL_CHECK_LEN; i++)
		{
			if (path_string[i] == '\0' || path_string[i] == '/' || path_string[i] == '\\')
				break;
			if (path_string[i] == ':' || path_string[i] == ';')
				break;
		}

		/* �����ˣ� ��ʾû������������� */
		if (path_string[i] == 0)
			goto must_be_unix;
		/* û�����꣬������������� */
		if ((path_string[i] == ':' || path_string[i] == ';') &&
			(path_string[i+1] == 0 || path_string[i + 1] == '/' || path_string[i + 1] == '\\'))
			goto loop_file_l2;

		/*
			��ô��ȷʵ��UNIX��
		*/
must_be_unix:
		fs = get_unix();
		if (fs == NULL)
			goto error;
		i = 1/*ȥ�� '/' */;
		cur_dir = fs->root_dir;				
		goto loop_file_l1;
	}

	/* 
		��������ķ��ţ���ȷ��·��������
	*/
	for (i = 0; i < FSS_VOLUMN_LABEL_CHECK_LEN; i++)
	{
		if (path_string[i] == '\0' || path_string[i] == '/' || path_string[i] == '\\')
			break;
		if (path_string[i] == ':' || path_string[i] == ';')
			break;
	}

	/* 
		����·����
	*/
loop_file_l2:
	if ((path_string[i] == ':' || path_string[i]==';') &&
		(path_string[i + 1] == 0 || path_string[i + 1] == '/' || path_string[i + 1] == '\\'))
	{
		struct fss_volumn	*fs;
		int k ;

		/*
			����·���������߼�����
		*/
		for (j = 0, k = 0; j <= i/*xxxx:,may have / before volumn name.*/; j++)
		{
			if (path_string[j] == '/' || path_string[j] == '\\')
				continue;
			interval[k] = path_string[j];						//copy drive id xxxx:
			k++;		
		}
		if (interval[k - 1] == ';') interval[k - 1] = ':';		//����Ǳ�̬��';'����ôʹ��':'�ɣ�
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
			�����ˣ�Ӧ�������·��
		*/
		j = i = 0;												//resolve from the head of the string
	}

	/* 
		�����ǣ���ôcur_dirû���ҵ�������Ǵ����·����ʽ
	*/
loop_file_l1:
	if (cur_dir == NULL)										//û�е�ǰ·���ɲ�����
		goto error;

	/*
		Ŀǰ���ҵ��˹��ص㣬������UNIX�Ļ��Ƿ�UNIX�ģ�ÿ������
		����һ��Ŀ¼�������ڶ��ҵ��˶�Ӧ���ϼ�Ŀ¼��

		��ʼ�ڸ�Ŀ¼�½��������·��.
		
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
		
		/*һЩС���ſ���ֱ������.*/
		if (j == i)
		{
			i++;				
			continue;
		}

		/*Look up*/
		tmp = cur_dir;

		cur_dir = lockup_file_in_this_dir(cur_dir, path_string, j, i);

		if (cur_dir == NULL)									//����ʧ�ܣ������û�������
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
		else										//�ҵ��ڵ㣬��ô�������еĴ�������
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

just_have_dir:
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

	/* Ŀ¼���е����� */
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
	v->root_dir = fss_file_new(NULL, root_private, "��Ŀ¼", FSS_FILE_TYPE_DIR);
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

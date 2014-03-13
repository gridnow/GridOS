/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *
 *   �����ļ�ϵͳ
 */
#ifndef DDK_VFS_H
#define DDK_VFS_H

#include <types.h>

struct fss_file;
struct fss_volumn;
struct fss_directory_entry;
struct fss_state_info;
typedef u64 file_pos;

/************************************************************************/
/* About block size                                                     */
/************************************************************************/
#define FSS_CACHE_DB_SIZE_SHIFT		(18)								/* 256kb */
#define FSS_CACHE_DB_SIZE			(1UL << FSS_CACHE_DB_SIZE_SHIFT)

/************************************************************************/
/* About VFS driver                                                     */
/************************************************************************/
struct fss_vfs_driver_ops
{
	/* Detect the file system if it can be driven by the driver */
	bool (*fDetect)(struct fss_volumn *volumn);
	
	/* Mount a file system and return the root directory */
	void *(*fMount)(struct fss_volumn *volumn);
	
	/* ���ļ������������Ը��ļ����ر����� */
	void *(*fOpen)(void *private_dir, const char *name);
	
	/* ��ȡ�ļ���Ϣ */
	int (*fStat)(void *private_file, struct fss_state_info *info);

	/* READ a block */
	int (*fRead)(void *private_file, unsigned long block_id, void *buffer, unsigned long *__in __out desired_size);
	
	/* WRITE a block */
	int (*fWrite)(void *private_file, unsigned long block_id, void *buffer, unsigned long *__in __out desired_size);
	
	/* Read the directory */
	int (*fReadDir)(void *private_file, struct fss_directory_entry *__in __out entries, int buffer_size);
	
	/* Clean a file, that means all data block allocated for the file is releases from disk */
	int (*fCleanFile)(void *private_file, bool errase);
	
	/*
	 �ض̻��������ļ���С
	 ����length��ʾ�ļ����³��ȣ�VFS��֤length������ļ����г������
	 */
	int (*fTruncate)(void *private_file, file_pos length);
	
	/* Create a file */
	int (*fCreateFile)(void *private_parent, char *fs_child_filename);
};

#define FSS_VFS_DRIVER_VERSION_CUR 0
struct fss_vfs_driver
{
	const char *name;
	int version;
	struct list_head list;
	struct fss_vfs_driver_ops * ops;
};

/**
	@brief Register a file system
 
	The file system driver use this method to register itself to the VFS layer.
	And the new driver will be called when a new partition is detected so as try to
	mount the partition.
 
	@param[in] who The file system driver descriptor
*/
void fss_vfs_register(struct fss_vfs_driver * who);


/***************************************************************************
 �й�Ŀ¼����
 **************************************************************************/
struct fss_directory_entry
{
	u64	ino;
	s64 off;
	unsigned short record_len;
	unsigned char type;
	char name[0]/* Allocated by provider */;
};

struct fss_state_info
{
	loff_t		size;
	unsigned long	blksize;
	unsigned long long	blocks;
};
#endif

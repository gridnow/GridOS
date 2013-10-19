/**
*  @defgroup posix_stat
*  @ingroup 标准C库编程接口
*
*  posix stat
*
*  @{
*/
#ifndef STAT_H
#define STAT_H

#include <types.h>

#define S_IFMT		0xf000	/* These bits determine file type */

/* File types */
#define S_IFREG		0x8000	/* Regular file */
#define S_IFBLK		0x6000	/* Block device file */
#define S_IFDIR		0x4000	/* Directory file */
#define	S_IFCHR		0x2000	/* Character device file */
#define S_IFIFO		0x1000	/* FIFO */
#define S_IFLNK		0xa000 	/* Symbol link */
#define S_IFSOCK	0xc000	/* Socket */

#define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
#define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
#define S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)

#if 0

struct stat
{
    dev_t st_dev;					/* Device. */
#if __WORDSIZE == 32
    unsigned short int __pad1;
#endif
#if __WORDSIZE == 64
    ino_t st_ino;					/* File serial number. */
#else
    ino_t __st_ino;					/* 32bit file serial number. */
#endif
#if __WORDSIZE == 32
    mode_t st_mode;					/* File mode. */
    nlink_t st_nlink;				/* Link count. */
#else
    nlink_t st_nlink;				/* Link count. */
    mode_t st_mode;					/* File mode. */
#endif
    uid_t st_uid;					/* User ID of the file's owner. */
    gid_t st_gid;					/* Group ID of the file's group.*/
#if __WORDSIZE == 64
    int __pad0;
#endif
	dev_t st_rdev;					/* Device number, if device. */
#if __WORDSIZE == 32
    unsigned short int __pad2;
#endif
#if __WORDSIZE == 64
	off_t st_size;					/* Size of file, in bytes. */
#else
    loff_t st_size;					/* Size of file, in bytes. */
#endif
    blksize_t st_blksize;			/* Optimal block size for I/O. */
#if __WORDSIZE == 64
    blkcnt_t st_blocks;				/* Number 512-byte blocks allocated. */
#else
    blkcnt64_t st_blocks;			/* Number 512-byte blocks allocated. */
#endif
#if defined __USE_MISC

    struct timespec st_atim;		/* Time of last access. */
    struct timespec st_mtim;		/* Time of last modification. */
    struct timespec st_ctim;		/* Time of last status change. */
# define st_atime st_atim.tv_sec	/* Backward compatibility. */
# define st_mtime st_mtim.tv_sec
# define st_ctime st_ctim.tv_sec
#else
    time_t st_atime;				/* Time of last access. */
    unsigned long int st_atimensec;	/* Nscecs of last access. */
    time_t st_mtime;				/* Time of last modification. */
    unsigned long int st_mtimensec;	/* Nsecs of last modification. */
    time_t st_ctime;				/* Time of last status change. */
    unsigned long int st_ctimensec;	/* Nsecs of last status change. */
#endif
#if __WORDSIZE == 64
    long int __unused[3];
#else
# ifndef __USE_FILE_OFFSET64
    unsigned long int __unused4;
    unsigned long int __unused5;
# else
    ino64_t st_ino;					/* File serial number.	*/
# endif
#endif
};

int fstat(int fd, struct stat *buf);
#endif

#endif

/** @} */
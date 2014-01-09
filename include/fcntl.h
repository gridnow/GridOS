/**
 *  @defgroup posix_fcntl
 *  @ingroup 标准C库编程接口
 *
 *  posix fcntl
 *
 *  @{
 */
#ifndef FCNTL_H
#define FCNTL_H

#include <arch/fcntl.h>

/* File access modes */
#define O_RDONLY	_O_RDONLY
#define O_WRONLY	_O_WRONLY
#define O_RDWR		_O_RDWR

/* File mask for file access modes */
#define O_ACCMODE	_O_ACCMODE

/* File status flags */
#define O_CREAT		_O_CREAT
#define O_EXCL		_O_EXCL
#define O_NOCTTY	_O_NOCTTY
#define O_TRUNC		_O_TRUNC
#define O_APPEND	_O_APPEND
#define O_NONBLOCK	_O_NONBLOCK
#define O_NDELAY	_O_NDELAY

#endif

/** @} */
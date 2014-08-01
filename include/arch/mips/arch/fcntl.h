#ifndef _MIPS_BITS_FCNTL_H
#define _MIPS_BITS_FCNTL_H

/* File access modes */
#define _O_RDONLY	00			/* Open read-only */
#define _O_WRONLY	01			/* Open write-only */
#define _O_RDWR		02			/* Open read/write */

/* File mask for file access modes */
#define _O_ACCMODE	(_O_RDONLY | _O_WRONLY | _O_RDWR)

/* File status flags */
#define _O_CREAT	0x0100		/* Create file if it doesn't exist */
#define _O_EXCL		0x0400		/* Fail if file already exists */
#define _O_NOCTTY	0x0800		/* Don't assign a controlling terminal */
#define _O_TRUNC	0x0200		/* Truncate file to zero length */
#define _O_APPEND	0x0008		/* Writes append to the file */
#define _O_NONBLOCK	0x0080		/* Non-blocking I/O */
#define _O_NDELAY	_O_NONBLOCK

#endif	/* _MIPS_BITS_FCNTL_H */
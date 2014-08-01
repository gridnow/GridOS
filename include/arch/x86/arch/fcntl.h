#ifndef _X86_BITS_FCNTL_H
#define _X86_BITS_FCNTL_H

/* File access modes */
#define _O_RDONLY	00			/* Open read-only */
#define _O_WRONLY	01			/* Open write-only */
#define _O_RDWR		02			/* Open read/write */

/* File mask for file access modes */
#define _O_ACCMODE	(_O_RDONLY | _O_WRONLY | _O_RDWR)

/* File status flags */
#define _O_CREAT	00100		/* Create file if it doesn't exist */
#define _O_EXCL		00200		/* Fail if file already exists */
#define _O_NOCTTY	00400		/* Don't assign a controlling terminal */
#define _O_TRUNC	01000		/* Truncate file to zero length */
#define _O_APPEND	02000		/* Writes append to the file */
#define _O_NONBLOCK	04000		/* Non-blocking I/O */
#define _O_NDELAY	_O_NONBLOCK

#endif	/* _X86_BITS_FCNTL_H */
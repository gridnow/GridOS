/**
	The Grid Core Library
 */

/**
	The most common definition
	Yaosihai
 */

#ifndef __CRT_H__
#define __CRT_H__

/************************************************************************/
/* Memory part                                                          */
/************************************************************************/
void *crt_free(void *p);
void *crt_alloc(int size);
void *crt_zalloc(int size);

/************************************************************************/
/* Error number part                                                    */
/************************************************************************/
void set_errno(int posix_errno);
int get_errno();

/************************************************************************/
/* Misc                                                                 */
/************************************************************************/
#define CRT_SPIN_LOCK(L)
#define CRT_SPIN_UNLOCK(L)
typedef long crt_lock_t;
#endif
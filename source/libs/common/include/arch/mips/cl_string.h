#ifndef _ASM_STRING_H
#define _ASM_STRING_H

/*
 * Most of the inline functions are rather naive implementations so I just
 * didn't bother updating them for 64-bit ...
 */
#ifdef CONFIG_32BIT
#define __HAVE_ARCH_STRCPY
char *strcpy(char *__dest, __const__ char *__src);
#define __HAVE_ARCH_STRNCPY
char *strncpy(char *__dest, __const__ char *__src, size_t __n);
#define __HAVE_ARCH_STRCMP
int strcmp(__const__ char *__cs, __const__ char *__ct);
#define __HAVE_ARCH_STRNCMP
int strncmp(__const__ char *__cs, __const__ char *__ct, size_t __count);
#endif

#define __HAVE_ARCH_MEMSET
extern void *memset(void *__s, int __c, size_t __count);

#define __HAVE_ARCH_MEMCPY
extern void *memcpy(void *__to, __const__ void *__from, size_t __n);

#define __HAVE_ARCH_MEMMOVE
extern void *memmove(void *__dest, __const__ void *__src, size_t __n);

#endif /* _ASM_STRING_H */

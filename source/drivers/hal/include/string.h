/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL 内部字符串操作接口
*/

#ifndef HAL_STRING_H
#define HAL_STRING_H

#include <types.h>
#include <asm/string.h>

#define isxdigit(c)	(('0' <= (c) && (c) <= '9') \
			 || ('a' <= (c) && (c) <= 'f') \
			 || ('A' <= (c) && (c) <= 'F'))

#define isdigit(c)	('0' <= (c) && (c) <= '9')
#define islower(c)	('a' <= (c) && (c) <= 'z')
#define toupper(c)	(islower(c) ? ((c) - 'a' + 'A') : (c))
#define toupper(c)	(islower(c) ? ((c) - 'a' + 'A') : (c))
#define isspace(c)	((c) == ' ')
#define TOLOWER(x)	((x) | 0x20)

/* String function */
#ifndef __HAVE_ARCH_SKIP_SPACE
char *skip_spaces(const char *str);
#endif

#ifndef __HAVE_ARCH_STRNCPY
char *strncpy(char *dest, const char *src, size_t count);
#endif

#ifndef __HAVE_ARCH_STRLEN
size_t strlen(const char *s);
#endif

#ifndef __HAVE_ARCH_STRCAT
char *strcat(char *dest, const char *src);
#endif

#ifndef __HAVE_ARCH_STRSTR
char *strstr(const char *s1, const char *s2);
#endif

#ifndef __HAVE_ARCH_STRCMP
int strcmp(const char *cs, const char *ct);
#endif

#ifndef __HAVE_ARCH_STRNCMP
int strncmp(const char *cs, const char *ct, size_t size);
#endif

#ifndef __HAVE_ARCH_STRRCHR
char *strrchr(const char *s, int c);
#endif

#ifndef __HAVE_ARCH_MEMCMP
int memcmp(const void *cs, const void *ct, size_t count);
#endif 

#ifndef __HAVE_ARCH_STRCPY
char *strcpy(char *dest, const char *src);
#endif

#ifdef __HAVE_ARCH_STRNCAT
char *strncat(char *dest, const char *src, size_t count);
#endif

unsigned long long simple_strtoull(const char *cp, char **endp, unsigned int base);
unsigned long simple_strtoul(const char *cp, char **endp, unsigned int base);
void *memcpys(unsigned char *__d, unsigned char *__s, int __n);
#endif


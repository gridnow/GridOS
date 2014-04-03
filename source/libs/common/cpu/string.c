/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   wuxin
 *   String 操作，非汇编优化版本
 */

/* Must from hal */
#include <string.h>

#ifndef __HAVE_ARCH_STRLEN
/**
 * strlen - Find the length of a string
 * @s: The string to be sized
 */
size_t strlen(const char *s)
{
	const char *sc;
	
	for (sc = s; *sc != '\0'; ++sc)
	/* nothing */;
	return sc - s;
}

#endif

#ifndef __HAVE_ARCH_STRNLEN
size_t strnlen(const char *s, size_t count)
{
	const char *sc;
	
	for (sc = s; count-- && *sc != '\0'; ++sc)
	/* nothing */;
	return sc - s;
}
#endif

#ifndef __HAVE_ARCH_STRCMP
#undef strcmp
int strcmp(const char *cs, const char *ct)
{
	unsigned char c1, c2;
	
	while (1) {
		c1 = *cs++;
		c2 = *ct++;
		if (c1 != c2)
			return c1 < c2 ? -1 : 1;
		if (!c1)
			break;
	}
	return 0;
}
#endif

#ifndef __HAVE_ARCH_STRCPY
char *strcpy(char *dest, const char *src)
{
	char *tmp = dest;
	
	while ((*dest++ = *src++) != '\0')
	/* nothing */;
	return tmp;
}
#endif

#ifndef __HAVE_ARCH_STRNCPY
char *strncpy(char *dest, const char *src, size_t count)
{
	char *tmp = dest;
	
	while (count) {
		if ((*tmp = *src) != 0)
			src++;
		tmp++;
		count--;
	}
	return dest;
}
#endif

#ifndef __HAVE_ARCH_STRNCMP
int strncmp(const char *cs, const char *ct, size_t count)
{
	unsigned char c1, c2;
	
	while (count) {
		c1 = *cs++;
		c2 = *ct++;
		if (c1 != c2)
			return c1 < c2 ? -1 : 1;
		if (!c1)
			break;
		count--;
	}
	return 0;
}
#endif

#ifndef __HAVE_ARCH_STRRCHR
char *strrchr(const char *s, int c)
{
	const char *p = s + strlen(s);
	do {
		if (*p == (char)c)
			return (char *)p;
	} while (--p >= s);
	return NULL;
}
#endif
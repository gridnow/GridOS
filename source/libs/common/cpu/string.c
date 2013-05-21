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
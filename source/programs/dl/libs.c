/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *
 *   ��̬������ ������
 */

#include <types.h>
#include <compiler.h>

#include "sys/ke_req.h"

__weak int strcmp(const char *cs, const char *ct)
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

__weak void *memset(void *dst, int value, unsigned long count)
{
	void *start = dst;
	
	while (count--)
	{
		*(char *)dst = (char)value;
		dst = (char *)dst + 1;
	}
	
	return start;
}

__weak void *memcpy(void *dst, const void *src, unsigned long len)
{
	void *ret = dst;
	
	while(len--)
	{
		*(char *)dst = *(char *)src;
		
		dst = (char *)dst + 1;
		src = (char *)src + 1;
	}
	
	return ret;
}
__weak size_t strlen(const char *s)
{
	const char *sc;
	
	for (sc = s; *sc != '\0'; ++sc)
	/* nothing */;
	return sc - s;
}

__weak size_t strnlen(const char *s, size_t maxlen)
{
	const char *es = s;
	while (*es && maxlen) {
		es++;
		maxlen--;
	}
	
	return (es - s);
}


__weak char *strrchr(const char *s, int c)
{
	const char *p = s + strlen(s);
	do {
		if (*p == (char)c)
			return (char *)p;
	} while (--p >= s);
	return NULL;
}

__weak char * strchr (const char * string, int ch)
{
	while (*string && *string != (char)ch)
		string++;
	
	if (*string == (char)ch)
		return((char *)string);
	return(NULL);
}


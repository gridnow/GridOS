#include <types.h>
#include <string.h>
#include <compiler.h>

DLLEXPORT __weak char *strrchr(const char *s, int c)
{
	const char *p = s + strlen(s);
	do {
		if (*p == (char)c)
			return (char *)p;
	} while (--p >= s);
	return NULL;
}

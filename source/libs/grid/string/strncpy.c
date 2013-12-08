#include <compiler.h>


DLLEXPORT __weak char *strncpy (char *dest, const char *src, int len)
{
	char *s = dest;

	while ((len--) && (*dest++ = *src++) )
		;

	return s;
}

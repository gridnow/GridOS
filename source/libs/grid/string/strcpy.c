#include <string.h>
#include <compiler.h>

DLLEXPORT __weak char *strcpy (char *dest, const char *src)
{
	char *s = dest;

	while (*dest++ = *src++)
		;

	return s;
}

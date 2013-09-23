#include <types.h>
#include <compiler.h>

DLLEXPORT __weak char * strchr (const char * string, int ch)
{
	while (*string && *string != (char)ch)
		string++;

	if (*string == (char)ch)
		return((char *)string);
	return(NULL);
}
#include <types.h>
#include <compiler.h>
#include <string.h>

DLLEXPORT __weak unsigned long strlen(const char * str)
{
	const char *eos = str;
	
	while (*eos++);
	
	return (eos - str - 1);
}

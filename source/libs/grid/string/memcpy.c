#include <compiler.h>

DLLEXPORT __weak void *memcpy(void *dst, const void *src, unsigned long len)
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

DLLEXPORT __weak void *memset(void *dst, int value, unsigned long count)
{
	void *start = dst;

	while (count--)
	{
		*(char *)dst = (char)value;
		dst = (char *)dst + 1;
	}

	return start;
}

/* 32bit memset */
#include <types.h>
#include <string.h>
void *memcpy(void *to, const void *from, size_t n)
{
#ifdef CONFIG_X86_USE_3DNOW
	return __memcpy3d(to, from, n);
#else
	return __memcpy(to, from, n);
#endif
}

void *memset(void *s, int c, size_t count)
{
	return __memset(s, c, count);
}

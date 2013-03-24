/* 32bit STRING OPS */
#include <string.h>

#ifdef __HAVE_ARCH_STRCPY
char *strcpy(char *dest, const char *src)
{
	int d0, d1, d2;
	asm volatile("1:\tlodsb\n\t"
		"stosb\n\t"
		"testb %%al,%%al\n\t"
		"jne 1b"
		: "=&S" (d0), "=&D" (d1), "=&a" (d2)
		: "0" (src), "1" (dest) : "memory");
	return dest;
}
#endif

#ifdef __HAVE_ARCH_STRNCPY
char *strncpy(char *dest, const char *src, size_t count)
{
	int d0, d1, d2, d3;
	asm volatile("1:\tdecl %2\n\t"
		"js 2f\n\t"
		"lodsb\n\t"
		"stosb\n\t"
		"testb %%al,%%al\n\t"
		"jne 1b\n\t"
		"rep\n\t"
		"stosb\n"
		"2:"
		: "=&S" (d0), "=&D" (d1), "=&c" (d2), "=&a" (d3)
		: "0" (src), "1" (dest), "2" (count) : "memory");
	return dest;
}
#endif

#ifdef __HAVE_ARCH_STRCAT
char *strcat(char *dest, const char *src)
{
	int d0, d1, d2, d3;
	asm volatile("repne\n\t"
		"scasb\n\t"
		"decl %1\n"
		"1:\tlodsb\n\t"
		"stosb\n\t"
		"testb %%al,%%al\n\t"
		"jne 1b"
		: "=&S" (d0), "=&D" (d1), "=&a" (d2), "=&c" (d3)
		: "0" (src), "1" (dest), "2" (0), "3" (0xffffffffu) : "memory");
	return dest;
}
#endif

#ifdef __HAVE_ARCH_STRNCAT
char *strncat(char *dest, const char *src, size_t count)
{
	int d0, d1, d2, d3;
	asm volatile("repne\n\t"
		"scasb\n\t"
		"decl %1\n\t"
		"movl %8,%3\n"
		"1:\tdecl %3\n\t"
		"js 2f\n\t"
		"lodsb\n\t"
		"stosb\n\t"
		"testb %%al,%%al\n\t"
		"jne 1b\n"
		"2:\txorl %2,%2\n\t"
		"stosb"
		: "=&S" (d0), "=&D" (d1), "=&a" (d2), "=&c" (d3)
		: "0" (src), "1" (dest), "2" (0), "3" (0xffffffffu), "g" (count)
		: "memory");
	return dest;
}
#endif

#ifdef __HAVE_ARCH_STRCMP
int strcmp(const char *cs, const char *ct)
{
	int d0, d1;
	int res;
	asm volatile("1:\tlodsb\n\t"
		"scasb\n\t"
		"jne 2f\n\t"
		"testb %%al,%%al\n\t"
		"jne 1b\n\t"
		"xorl %%eax,%%eax\n\t"
		"jmp 3f\n"
		"2:\tsbbl %%eax,%%eax\n\t"
		"orb $1,%%al\n"
		"3:"
		: "=a" (res), "=&S" (d0), "=&D" (d1)
		: "1" (cs), "2" (ct)
		: "memory");
	return res;
}
#endif

#ifdef __HAVE_ARCH_STRNCMP
int strncmp(const char *cs, const char *ct, size_t count)
{
	int res;
	int d0, d1, d2;
	asm volatile("1:\tdecl %3\n\t"
		"js 2f\n\t"
		"lodsb\n\t"
		"scasb\n\t"
		"jne 3f\n\t"
		"testb %%al,%%al\n\t"
		"jne 1b\n"
		"2:\txorl %%eax,%%eax\n\t"
		"jmp 4f\n"
		"3:\tsbbl %%eax,%%eax\n\t"
		"orb $1,%%al\n"
		"4:"
		: "=a" (res), "=&S" (d0), "=&D" (d1), "=&c" (d2)
		: "1" (cs), "2" (ct), "3" (count)
		: "memory");
	return res;
}
#endif

#ifdef __HAVE_ARCH_STRCHR
char *strchr(const char *s, int c)
{
	int d0;
	char *res;
	asm volatile("movb %%al,%%ah\n"
		"1:\tlodsb\n\t"
		"cmpb %%ah,%%al\n\t"
		"je 2f\n\t"
		"testb %%al,%%al\n\t"
		"jne 1b\n\t"
		"movl $1,%1\n"
		"2:\tmovl %1,%0\n\t"
		"decl %0"
		: "=a" (res), "=&S" (d0)
		: "1" (s), "0" (c)
		: "memory");
	return res;
}
#endif

#ifdef __HAVE_ARCH_STRLEN
size_t strlen(const char *s)
{
	int d0;
	int res;
	asm volatile("repne\n\t"
		"scasb\n\t"
		"notl %0\n\t"
		"decl %0"
		: "=c" (res), "=&D" (d0)
		: "1" (s), "a" (0), "0" (0xffffffffu)
		: "memory");
	return res;
}
#endif

#ifdef __HAVE_ARCH_MEMCHR
void *memchr(const void *cs, int c, size_t count)
{
	int d0;
	void *res;
	if (!count)
		return NULL;
	asm volatile("repne\n\t"
		"scasb\n\t"
		"je 1f\n\t"
		"movl $1,%0\n"
		"1:\tdecl %0"
		: "=D" (res), "=&c" (d0)
		: "a" (c), "0" (cs), "1" (count)
		: "memory");
	return res;
}

#endif

#ifdef __HAVE_ARCH_MEMSCAN
void *memscan(void *addr, int c, size_t size)
{
	if (!size)
		return addr;
	asm volatile("repnz; scasb\n\t"
	    "jnz 1f\n\t"
	    "dec %%edi\n"
	    "1:"
	    : "=D" (addr), "=c" (size)
	    : "0" (addr), "1" (size), "a" (c)
	    : "memory");
	return addr;
}
#endif

#ifdef __HAVE_ARCH_STRNLEN
size_t strnlen(const char *s, size_t count)
{
	int d0;
	int res;
	asm volatile("movl %2,%0\n\t"
		"jmp 2f\n"
		"1:\tcmpb $0,(%0)\n\t"
		"je 3f\n\t"
		"incl %0\n"
		"2:\tdecl %1\n\t"
		"cmpl $-1,%1\n\t"
		"jne 1b\n"
		"3:\tsubl %2,%0"
		: "=a" (res), "=&d" (d0)
		: "c" (s), "1" (count)
		: "memory");
	return res;
}
#endif

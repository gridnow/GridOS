#include "string.h"

#ifdef CONFIG_32BIT
char *strcpy(char *__dest, __const__ char *__src)
{
  char *__xdest = __dest;

  __asm__ __volatile__(
	".set\tnoreorder\n\t"
	".set\tnoat\n"
	"1:\tlbu\t$1,(%1)\n\t"
	"addiu\t%1,1\n\t"
	"sb\t$1,(%0)\n\t"
	"bnez\t$1,1b\n\t"
	"addiu\t%0,1\n\t"
	".set\tat\n\t"
	".set\treorder"
	: "=r" (__dest), "=r" (__src)
        : "0" (__dest), "1" (__src)
	: "memory");

  return __xdest;
}

char *strncpy(char *__dest, __const__ char *__src, size_t __n)
{
  char *__xdest = __dest;

  if (__n == 0)
    return __xdest;

  __asm__ __volatile__(
	".set\tnoreorder\n\t"
	".set\tnoat\n"
	"1:\tlbu\t$1,(%1)\n\t"
	"subu\t%2,1\n\t"
	"sb\t$1,(%0)\n\t"
	"beqz\t$1,2f\n\t"
	"addiu\t%0,1\n\t"
	"bnez\t%2,1b\n\t"
	"addiu\t%1,1\n"
	"2:\n\t"
	".set\tat\n\t"
	".set\treorder"
        : "=r" (__dest), "=r" (__src), "=r" (__n)
        : "0" (__dest), "1" (__src), "2" (__n)
        : "memory");

  return __xdest;
}

int strcmp(__const__ char *__cs, __const__ char *__ct)
{
  int __res;

  __asm__ __volatile__(
	".set\tnoreorder\n\t"
	".set\tnoat\n\t"
	"lbu\t%2,(%0)\n"
	"1:\tlbu\t$1,(%1)\n\t"
	"addiu\t%0,1\n\t"
	"bne\t$1,%2,2f\n\t"
	"addiu\t%1,1\n\t"
	"bnez\t%2,1b\n\t"
	"lbu\t%2,(%0)\n\t"
#if defined(CONFIG_CPU_R3000)
	"nop\n\t"
#endif
	"move\t%2,$1\n"
	"2:\tsubu\t%2,$1\n"
	"3:\t.set\tat\n\t"
	".set\treorder"
	: "=r" (__cs), "=r" (__ct), "=r" (__res)
	: "0" (__cs), "1" (__ct));

  return __res;
}

int strncmp(__const__ char *__cs, __const__ char *__ct, size_t __count)
{
	int __res;

	__asm__ __volatile__(
	".set\tnoreorder\n\t"
	".set\tnoat\n"
	"1:\tlbu\t%3,(%0)\n\t"
	"beqz\t%2,2f\n\t"
	"lbu\t$1,(%1)\n\t"
	"subu\t%2,1\n\t"
	"bne\t$1,%3,3f\n\t"
	"addiu\t%0,1\n\t"
	"bnez\t%3,1b\n\t"
	"addiu\t%1,1\n"
	"2:\n\t"
#if defined(CONFIG_CPU_R3000)
	"nop\n\t"
#endif
	"move\t%3,$1\n"
	"3:\tsubu\t%3,$1\n\t"
	".set\tat\n\t"
	".set\treorder"
	: "=r" (__cs), "=r" (__ct), "=r" (__count), "=r" (__res)
	: "0" (__cs), "1" (__ct), "2" (__count));

	return __res;
}
#endif /* CONFIG_32BIT */

#ifndef IMPLEMENT_H
#define IMPLEMENT_H

/*
 * implement.h
 *
 * Definitions that don't need to be public.
 *
 * Keeps all the internals out of pthread.h
 */

#if !defined(ETIMEDOUT)
#  define ETIMEDOUT 10060	/* This is the value in winsock.h. */
#endif

#define PTW32_INTERLOCKED_LONG long
#define PTW32_INTERLOCKED_INT int
#define PTW32_INTERLOCKED_SIZE size_t
#define PTW32_INTERLOCKED_VOLATILE volatile

/************************************************************************/
/* ABOUT SPIN                                                           */
/************************************************************************/

#if defined(__GNUC__) 

#ifndef ARCH_ATOMIC_BARRIER
#define ARCH_ATOMIC_BARRIER  __asm volatile("" ::: "memory")
#endif

#endif /* GNU */
#endif /* IMPLEMENT_H */
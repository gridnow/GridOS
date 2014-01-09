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
#define PTW32_SPIN_INVALID     (0)
#define PTW32_SPIN_UNLOCKED    (1)
#define PTW32_SPIN_LOCKED      (2)

#if defined(__GNUC__) 
#if defined(__i386__)
# if defined(_WIN64)
# define PTW32_INTERLOCKED_COMPARE_EXCHANGE_64(location, value, comparand)    \
    ({                                                                     \
      __typeof (value) _result;                                            \
      __asm__ __volatile__                                                 \
      (                                                                    \
        "lock\n\t"                                                         \
        "cmpxchgq      %2,(%1)"                                            \
        :"=a" (_result)                                                    \
        :"r"  (location), "r" (value), "a" (comparand)                     \
        :"memory", "cc");                                                  \
      _result;                                                             \
    })
# define PTW32_INTERLOCKED_EXCHANGE_64(location, value)                    \
    ({                                                                     \
      __typeof (value) _result;                                            \
      __asm__ __volatile__                                                 \
      (                                                                    \
        "xchgq	 %0,(%1)"                                                  \
        :"=r" (_result)                                                    \
        :"r" (location), "0" (value)                                       \
        :"memory", "cc");                                                  \
      _result;                                                             \
    })
# define PTW32_INTERLOCKED_EXCHANGE_ADD_64(location, value)                \
    ({                                                                     \
      __typeof (value) _result;                                            \
      __asm__ __volatile__                                                 \
      (                                                                    \
        "lock\n\t"                                                         \
        "xaddq	 %0,(%1)"                                                  \
        :"=r" (_result)                                                    \
        :"r" (location), "0" (value)                                       \
        :"memory", "cc");                                                  \
      _result;                                                             \
    })
# define PTW32_INTERLOCKED_INCREMENT_64(location)                          \
    ({                                                                     \
      PTW32_INTERLOCKED_LONG _temp = 1;                                   \
      __asm__ __volatile__                                                 \
      (                                                                    \
        "lock\n\t"                                                         \
        "xaddq	 %0,(%1)"                                                  \
        :"+r" (_temp)                                                      \
        :"r" (location)                                                    \
        :"memory", "cc");                                                  \
      ++_temp;                                                             \
    })
# define PTW32_INTERLOCKED_DECREMENT_64(location)                          \
    ({                                                                     \
      PTW32_INTERLOCKED_LONG _temp = -1;                                  \
      __asm__ __volatile__                                                 \
      (                                                                    \
        "lock\n\t"                                                         \
        "xaddq	 %2,(%1)"                                                  \
        :"+r" (_temp)                                                      \
        :"r" (location)                                                    \
        :"memory", "cc");                                                  \
      --_temp;                                                             \
    })
#endif
# define PTW32_INTERLOCKED_COMPARE_EXCHANGE(location, value, comparand)    \
    ({                                                                     \
      __typeof (value) _result;                                            \
      __asm__ __volatile__                                                 \
      (                                                                    \
        "lock\n\t"                                                         \
        "cmpxchgl       %2,(%1)"                                           \
        :"=a" (_result)                                                    \
        :"r"  (location), "r" (value), "a" (comparand)                     \
        :"memory", "cc");                                                  \
      _result;                                                             \
    })
# define PTW32_INTERLOCKED_EXCHANGE(location, value)                  \
    ({                                                                     \
      __typeof (value) _result;                                            \
      __asm__ __volatile__                                                 \
      (                                                                    \
        "xchgl	 %0,(%1)"                                                  \
        :"=r" (_result)                                                    \
        :"r" (location), "0" (value)                                       \
        :"memory", "cc");                                                  \
      _result;                                                             \
    })
# define PTW32_INTERLOCKED_EXCHANGE_ADD(location, value)              \
    ({                                                                     \
      __typeof (value) _result;                                            \
      __asm__ __volatile__                                                 \
      (                                                                    \
        "lock\n\t"                                                         \
        "xaddl	 %0,(%1)"                                                  \
        :"=r" (_result)                                                    \
        :"r" (location), "0" (value)                                       \
        :"memory", "cc");                                                  \
      _result;                                                             \
    })
# define PTW32_INTERLOCKED_INCREMENT(location)                        \
    ({                                                                     \
      PTW32_INTERLOCKED_INT _temp = 1;                                   \
      __asm__ __volatile__                                                 \
      (                                                                    \
        "lock\n\t"                                                         \
        "xaddl	 %0,(%1)"                                                  \
        :"+r" (_temp)                                                      \
        :"r" (location)                                                    \
        :"memory", "cc");                                                  \
      ++_temp;                                                             \
    })
# define PTW32_INTERLOCKED_DECREMENT(location)                        \
    ({                                                                     \
      PTW32_INTERLOCKED_INT _temp = -1;                                  \
      __asm__ __volatile__                                                 \
      (                                                                    \
        "lock\n\t"                                                         \
        "xaddl	 %0,(%1)"                                                  \
        :"+r" (_temp)                                                      \
        :"r" (location)                                                    \
        :"memory", "cc");                                                  \
      --_temp;                                                             \
    })
#elif defined (__mips__)
#define PTW32_INTERLOCKED_COMPARE_EXCHANGE(location, value, comparand)   (0)
#endif

#ifndef ARCH_ATOMIC_BARRIER
#define ARCH_ATOMIC_BARRIER  __asm volatile("" ::: "memory")
#endif

#endif /* GNU */
#endif /* IMPLEMENT_H */
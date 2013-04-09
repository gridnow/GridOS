/*
	OLD driver need this
*/
#ifndef DDK_COMPATIBLE_INTERFACE_ATOMIC
#define DDK_COMPATIBLE_INTERFACE_ATOMIC

#include <kernel/ke_atomic.h>

#define atomic_dec ke_atomic_dec
#define atomic_add_return(c, a) ke_atomic_add_return(a, c)
#define atomic_inc_return(v)	ke_atomic_add_return(1, (v))
#define atomic_sub_return(c, a) ke_atomic_sub_return(a, c)

typedef struct ke_atomic atomic_t;

#endif


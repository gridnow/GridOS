/**
*  @defgroup DDK
*
*	驱动开发API
*  @{
*/
/** @} */

/**
*  @defgroup types
*  @ingroup DDK
*
*  定义了DDK的关键类型
*  @{
*/

#ifndef _DDK_TYPES_H_
#define _DDK_TYPES_H_

/* Base types */
#include <types.h>
typedef u64 phys_addr_t;
typedef u64 dma_addr_t;
//typedef unsigned  gfp_t;

#ifdef __CHECKER__
#define __bitwise__ __attribute__((bitwise))
#else
#define __bitwise__
#endif
#ifdef __CHECK_ENDIAN__
#define __bitwise __bitwise__
#else
#define __bitwise
#endif

typedef __u16 __bitwise __le16;
typedef __u16 __bitwise __be16;
typedef __u32 __bitwise __le32;
typedef __u32 __bitwise __be32;

typedef __u16 __bitwise __sum16;
typedef __u32 __bitwise __wsum;

#endif

/** @} */
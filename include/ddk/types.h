/**
*  @defgroup DDK
*
*	��������API
*  @{
*/
/** @} */

/**
*  @defgroup types
*  @ingroup DDK
*
*  ������DDK�Ĺؼ�����
*  @{
*/

#ifndef _DDK_TYPES_H_
#define _DDK_TYPES_H_

/* Base types */
#include <types.h>
typedef u64 phys_addr_t;
typedef unsigned long dma_addr_t;

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
typedef u64 __bitwise __le64;
typedef u64 __bitwise __be64;

typedef __u16 __bitwise __sum16;
typedef __u32 __bitwise __wsum;

#endif

/** @} */

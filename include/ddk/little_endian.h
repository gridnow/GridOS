#ifndef _BYTEORDER_LITTLE_ENDIAN_H
#define _BYTEORDER_LITTLE_ENDIAN_H

#include "swab.h"
#include "compiler.h"

#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN 1234
#endif
#ifndef __LITTLE_ENDIAN_BITFIELD
#define __LITTLE_ENDIAN_BITFIELD
#endif

#define __BYTE_ORDER __LITTLE_ENDIAN

#define __constant_cpu_to_le32(x) ((__force __le32)(__u32)(x))
#define __constant_cpu_to_be32(x) ((__force __be32)___constant_swab32((x)))

#define __cpu_to_le16(x) ((__force __le16)(__u16)(x))
#define __cpu_to_le32(x) ((__force __le32)(__u32)(x))
#define __cpu_to_le64(x) ((__force __le64)(u64)(x))

#define __le16_to_cpu(x) ((__force __u16)(__le16)(x))
#define __le32_to_cpu(x) ((__force __u32)(__le32)(x))
#define __le64_to_cpu(x) ((__force u64)(__le64)(x))

#define __cpu_to_be32(x) ((__force __be32)__swab32((x)))
#define __be32_to_cpu(x) __swab32((__force __u32)(__be32)(x))

/* Generic(.h) interface */
#define le64_to_cpu __le64_to_cpu
#define le16_to_cpu __le16_to_cpu
#define cpu_to_le16 __cpu_to_le16
#define le32_to_cpu __le32_to_cpu
#define cpu_to_le32 __cpu_to_le32
#define cpu_to_le64 __cpu_to_le64
#endif /* __BYTEORDER_LITTLE_ENDIAN_H */

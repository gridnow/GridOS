#ifndef _BYTEORDER_LITTLE_ENDIAN_H
#define _BYTEORDER_LITTLE_ENDIAN_H

#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN 1234
#endif
#ifndef __LITTLE_ENDIAN_BITFIELD
#define __LITTLE_ENDIAN_BITFIELD
#endif

#define __BYTE_ORDER __LITTLE_ENDIAN
#define __cpu_to_le32(x) ((__force __le32)(__u32)(x))
#define __cpu_to_le16(x) ((__force __le16)(__u16)(x))
#define __le32_to_cpu(x) ((__force __u32)(__le32)(x))

#define __le16_to_cpu(x) ((__force __u16)(__le16)(x))

/* Generial interface */
#define cpu_to_le16 __cpu_to_le16
#endif /* __BYTEORDER_LITTLE_ENDIAN_H */

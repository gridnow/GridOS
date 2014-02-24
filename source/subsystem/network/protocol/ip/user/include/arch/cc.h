#ifndef __ARCH_CC_H__
#define __ARCH_CC_H__

#include <stdio.h> /* printf, fflush, FILE */
#include <stdlib.h> /* abort */
#include <limits.h>

#ifdef _MSC_VER
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSTcpIP.h>
#pragma warning (disable: 4244) /* disable conversion warning (implicit integer promotion!) */
#pragma warning (disable: 4127) /* conditional expression is constant */
#pragma warning (disable: 4996) /* 'strncpy' was declared deprecated */
#pragma warning (disable: 4103) /* structure packing changed by including file */
#endif

/* Define platform endianness (might already be defined) */
#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif /* BYTE_ORDER */

/* Define generic types used in lwIP */
typedef unsigned   char    u8_t;
typedef signed     char    s8_t;
typedef unsigned   short   u16_t;
typedef signed     short   s16_t;
typedef unsigned   int    u32_t;
typedef signed     int    s32_t;

/* Strange types */
typedef unsigned	char	u8;
typedef unsigned	int		u32;
typedef unsigned	int		__u32;
typedef unsigned	short	__u16;
typedef __u16				__be16;
typedef __u32				__be32;

typedef unsigned long mem_ptr_t;
typedef u32_t sys_prot_t;

/* Define (sn)printf formatters for these lwIP types */
#define X8_F  "02x"
#define U16_F "hu"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"

#ifdef __GNUC__
#define S16_F "d"
#define X16_F "x"
#define SZT_F "u"
#else
#define S16_F "d"
#define X16_F "x"
#define SZT_F "u" 
#endif

/* Compiler hints for packing structures */
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_USE_INCLUDES

/* Plaform specific diagnostic output */
#define LWIP_PLATFORM_DIAG(x)   do { printf x; } while(0)

#define LWIP_PLATFORM_ASSERT(x) do { printf("Assertion \"%s\" failed at line %d in %s\n", \
	x, __LINE__, __FILE__); fflush(NULL); {long *__p = 0; *__p = 0xdeadbeaf;} } while(0)

#define LWIP_ERROR(message, expression, handler) do { if (!(expression)) { \
	printf("Assertion \"%s\" failed at line %d in %s\n", message, __LINE__, __FILE__); \
	fflush(NULL);handler;} } while(0)

#ifdef _MSC_VER
/* C runtime functions redefined */
#define snprintf _snprintf
#define inline __inline
#endif

u32_t dns_lookup_external_hosts_file(const char *name);

#define LWIP_RAND() ((u32_t)rand())

#endif /* __ARCH_CC_H__ */

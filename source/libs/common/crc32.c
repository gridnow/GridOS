/*
 * Aug 8, 2011 Bob Pearson with help from Joakim Tjernlund and George Spelvin
 * cleaned up code to current version of sparse and added the slicing-by-8
 * algorithm to the closely similar existing slicing-by-4 algorithm.
 *
 * Oct 15, 2000 Matt Domsch <Matt_Domsch@dell.com>
 * Nicer crc32 functions/docs submitted by linux@horizon.com.  Thanks!
 * Code was from the public domain, copyright abandoned.  Code was
 * subsequently included in the kernel, thus was re-licensed under the
 * GNU GPL v2.
 *
 * Oct 12, 2000 Matt Domsch <Matt_Domsch@dell.com>
 */

#include <ddk/types.h>
#include <ddk/byteorder.h>

#include "crc32.h"
#include "crc32defs.h"

#if CRC_LE_BITS > 8
# define tole(x) ((__force u32) __constant_cpu_to_le32(x))
#else
# define tole(x) (x)
#endif

#if CRC_BE_BITS > 8
# define tobe(x) ((__force u32) __constant_cpu_to_be32(x))
#else
# define tobe(x) (x)
#endif

#define __cacheline_aligned //TODO: __cacheline_aligned should be auto selected
#include "crc32table.h"


#if CRC_LE_BITS > 8 || CRC_BE_BITS > 8

/* implements slicing-by-4 or slicing-by-8 algorithm */
static inline u32
crc32_body(u32 crc, unsigned char const *buf, size_t len, const u32 (*tab)[256])
{
# ifdef __LITTLE_ENDIAN
#  define DO_CRC(x) crc = t0[(crc ^ (x)) & 255] ^ (crc >> 8)
#  define DO_CRC4 (t3[(q) & 255] ^ t2[(q >> 8) & 255] ^ \
		   t1[(q >> 16) & 255] ^ t0[(q >> 24) & 255])
#  define DO_CRC8 (t7[(q) & 255] ^ t6[(q >> 8) & 255] ^ \
		   t5[(q >> 16) & 255] ^ t4[(q >> 24) & 255])
# else
#  define DO_CRC(x) crc = t0[((crc >> 24) ^ (x)) & 255] ^ (crc << 8)
#  define DO_CRC4 (t0[(q) & 255] ^ t1[(q >> 8) & 255] ^ \
		   t2[(q >> 16) & 255] ^ t3[(q >> 24) & 255])
#  define DO_CRC8 (t4[(q) & 255] ^ t5[(q >> 8) & 255] ^ \
		   t6[(q >> 16) & 255] ^ t7[(q >> 24) & 255])
# endif
	const u32 *b;
	size_t    rem_len;
# ifdef CONFIG_X86
	size_t i;
# endif
	const u32 *t0=tab[0], *t1=tab[1], *t2=tab[2], *t3=tab[3];
# if CRC_LE_BITS != 32
	const u32 *t4 = tab[4], *t5 = tab[5], *t6 = tab[6], *t7 = tab[7];
# endif
	u32 q;

	/* Align it */
	if (unlikely((long)buf & 3 && len)) {
		do {
			DO_CRC(*buf++);
		} while ((--len) && ((long)buf)&3);
	}

# if CRC_LE_BITS == 32
	rem_len = len & 3;
	len = len >> 2;
# else
	rem_len = len & 7;
	len = len >> 3;
# endif

	b = (const u32 *)buf;
# ifdef CONFIG_X86
	--b;
	for (i = 0; i < len; i++) {
# else
	for (--b; len; --len) {
# endif
		q = crc ^ *++b; /* use pre increment for speed */
# if CRC_LE_BITS == 32
		crc = DO_CRC4;
# else
		crc = DO_CRC8;
		q = *++b;
		crc ^= DO_CRC4;
# endif
	}
	len = rem_len;
	/* And the last few bytes */
	if (len) {
		u8 *p = (u8 *)(b + 1) - 1;
# ifdef CONFIG_X86
		for (i = 0; i < len; i++)
			DO_CRC(*++p); /* use pre increment for speed */
# else
		do {
			DO_CRC(*++p); /* use pre increment for speed */
		} while (--len);
# endif
	}
	return crc;
#undef DO_CRC
#undef DO_CRC4
#undef DO_CRC8
}
#endif

/**
 * crc32_le() - Calculate bitwise little-endian Ethernet AUTODIN II CRC32
 * @crc: seed value for computation.  ~0 for Ethernet, sometimes 0 for
 *	other uses, or the previous crc32 value if computing incrementally.
 * @p: pointer to buffer over which CRC is run
 * @len: length of buffer @p
 */
static inline u32 __pure crc32_le_generic(u32 crc, unsigned char const *p,
					  size_t len, const u32 (*tab)[256],
					  u32 polynomial)
{
#if CRC_LE_BITS == 1
	int i;
	while (len--) {
		crc ^= *p++;
		for (i = 0; i < 8; i++)
			crc = (crc >> 1) ^ ((crc & 1) ? polynomial : 0);
	}
# elif CRC_LE_BITS == 2
	while (len--) {
		crc ^= *p++;
		crc = (crc >> 2) ^ tab[0][crc & 3];
		crc = (crc >> 2) ^ tab[0][crc & 3];
		crc = (crc >> 2) ^ tab[0][crc & 3];
		crc = (crc >> 2) ^ tab[0][crc & 3];
	}
# elif CRC_LE_BITS == 4
	while (len--) {
		crc ^= *p++;
		crc = (crc >> 4) ^ tab[0][crc & 15];
		crc = (crc >> 4) ^ tab[0][crc & 15];
	}
# elif CRC_LE_BITS == 8
	/* aka Sarwate algorithm */
	while (len--) {
		crc ^= *p++;
		crc = (crc >> 8) ^ tab[0][crc & 255];
	}
# else
	crc = (__force u32) __cpu_to_le32(crc);
	crc = crc32_body(crc, p, len, tab);
	crc = __le32_to_cpu((__force __le32)crc);
#endif
	return crc;
}

#if CRC_LE_BITS == 1
u32 __pure crc32_le(u32 crc, unsigned char const *p, size_t len)
{
	return crc32_le_generic(crc, p, len, NULL, CRCPOLY_LE);
}
u32 __pure __crc32c_le(u32 crc, unsigned char const *p, size_t len)
{
	return crc32_le_generic(crc, p, len, NULL, CRC32C_POLY_LE);
}
#else
u32 __pure crc32_le(u32 crc, unsigned char const *p, size_t len)
{
	return crc32_le_generic(crc, p, len,
			(const u32 (*)[256])crc32table_le, CRCPOLY_LE);
}
u32 __pure __crc32c_le(u32 crc, unsigned char const *p, size_t len)
{
	return crc32_le_generic(crc, p, len,
			(const u32 (*)[256])crc32ctable_le, CRC32C_POLY_LE);
}
#endif

/**
 * crc32_be() - Calculate bitwise big-endian Ethernet AUTODIN II CRC32
 * @crc: seed value for computation.  ~0 for Ethernet, sometimes 0 for
 *	other uses, or the previous crc32 value if computing incrementally.
 * @p: pointer to buffer over which CRC is run
 * @len: length of buffer @p
 */
static inline u32 __pure crc32_be_generic(u32 crc, unsigned char const *p,
					  size_t len, const u32 (*tab)[256],
					  u32 polynomial)
{
#if CRC_BE_BITS == 1
	int i;
	while (len--) {
		crc ^= *p++ << 24;
		for (i = 0; i < 8; i++)
			crc =
			    (crc << 1) ^ ((crc & 0x80000000) ? polynomial :
					  0);
	}
# elif CRC_BE_BITS == 2
	while (len--) {
		crc ^= *p++ << 24;
		crc = (crc << 2) ^ tab[0][crc >> 30];
		crc = (crc << 2) ^ tab[0][crc >> 30];
		crc = (crc << 2) ^ tab[0][crc >> 30];
		crc = (crc << 2) ^ tab[0][crc >> 30];
	}
# elif CRC_BE_BITS == 4
	while (len--) {
		crc ^= *p++ << 24;
		crc = (crc << 4) ^ tab[0][crc >> 28];
		crc = (crc << 4) ^ tab[0][crc >> 28];
	}
# elif CRC_BE_BITS == 8
	while (len--) {
		crc ^= *p++ << 24;
		crc = (crc << 8) ^ tab[0][crc >> 24];
	}
# else
	crc = (__force u32) __cpu_to_be32(crc);
	crc = crc32_body(crc, p, len, tab);
	crc = __be32_to_cpu((__force __be32)crc);
# endif
	return crc;
}

#if CRC_LE_BITS == 1
u32 __pure crc32_be(u32 crc, unsigned char const *p, size_t len)
{
	return crc32_be_generic(crc, p, len, NULL, CRCPOLY_BE);
}
#else
u32 __pure crc32_be(u32 crc, unsigned char const *p, size_t len)
{
	return crc32_be_generic(crc, p, len,
			(const u32 (*)[256])crc32table_be, CRCPOLY_BE);
}
#endif

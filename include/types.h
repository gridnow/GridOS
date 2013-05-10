/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*/

/**
*  @defgroup BasicTypes
*  @ingroup ��׼C���̽ӿ�
*
*  ������������ã����ñ������Ͷ���
*
*  @{
*/
#ifndef TYPES_H
#define TYPES_H

/* C++ needs to know that types and declarations are C, not C++.  */
#ifdef	__cplusplus
# define BEGIN_C_DECLS	extern "C" {
# define END_C_DECLS	}
#else
# define BEGIN_C_DECLS
# define END_C_DECLS
#endif

BEGIN_C_DECLS;

#ifndef __ASSEMBLY__
typedef unsigned char		__u8;								/**< �޷���8λ���ݣ����鲻�øð汾������Ϊ�˼���һЩ��ͳ��� */
typedef unsigned char		u8;									/**< �޷���8λ���� */
typedef   signed char		s8;									/**< �з���8λ���� */
typedef unsigned short		__u16;								/**< �޷���16λ���ݣ����鲻�øð汾������Ϊ�˼���һЩ��ͳ��� */
typedef unsigned short		u16;								/**< �޷���16λ���� */
typedef unsigned int		__u32;								/**< �޷���32λ���ݣ����鲻�øð汾������Ϊ�˼���һЩ��ͳ��� */
typedef unsigned int		u32;								/**< �޷���32λ���� */
typedef signed   int		s32;								/**< �з���32λ���� */
typedef unsigned long		size_t;								/**< һ�����Ա�ʾһ�����������ȣ��޷��Ű汾 */
typedef long				ssize_t;							/**< һ�����Ա�ʾһ�����������ȣ��з��Ű汾 */

#if defined(__i386__)
typedef unsigned long long	u64;								/**< x86-32ƽ̨�ϵ��޷���64λ���� */
typedef signed long long s64;									/**< x86-32ƽ̨�ϵ��з���64λ���� */
#define BITS_PER_LONG		32									/**< x86-32ƽ̨�ϣ�һ��long�ͱ�����32λ */

#elif defined(__mips__)
	#ifdef __mips64
		typedef unsigned long u64;								/**< MIPS64ƽ̨�ϵ��޷���64λ���� */
		typedef signed long s64;								/**< MIPS64ƽ̨�ϵ��з���64λ���� */
		#define BITS_PER_LONG 64								/**< MIPS64ƽ̨�ϣ�һ��long�ͱ�����64λ */
	#else
		typedef unsigned long long	u64;
		typedef signed long long s64;
		#define BITS_PER_LONG 32
	#endif

#elif defined(__arm__)
typedef unsigned long long	u64;								/**< ARM ƽ̨�ϵ��޷���64λ���� */
typedef signed long long s64;									/**< ARM ƽ̨�ϵ��з���64λ���� */
#define BITS_PER_LONG		32									/**< ARM ƽ̨�ϣ�һ��long�ͱ�����32λ */

#else
#error "ƽ̨����������û�ж���"
#endif
/* ��̬��off_t */
typedef s64			off_t;
typedef s64			loff_t;

/* λ���� */
#define BITS_PER_BYTE              8
#define BITS_TO_LONGS(nr)          DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))
#define DIV_ROUND_UP(n,d)          (((n) + (d) - 1) / (d))
#define DECLARE_BITMAP(name,bits) \
	unsigned long name[BITS_TO_LONGS(bits)]

/* �ַ����ͣ�����ϵͳ���Լ������ֱ��뷽ʽ����˽���ʹ��xchar/xstring������ͳһ���� */
typedef char xchar;
typedef char * xstring;

/* bool */
#ifndef  __cplusplus
typedef unsigned int bool;
#define true 1
#define false 0
#endif
#endif /* __ASSEMBLY__ */

/* Pointer */
#define NULL 0

/* ������ */
#define __in
#define __out
#define __user 

END_C_DECLS;

#endif

/** @} */

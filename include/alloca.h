/**
 *  @defgroup STDLIB
 *  @ingroup ��׼C���̽ӿ�
 *
 *  ��׼������ӿڣ�ͷ�ļ���stdlib.h
 *
 *  @{
 */
#ifndef ALLOCA_H
#define ALLOCA_H

#undef alloca

#ifdef	__GNUC__
# define alloca(size)	__builtin_alloca (size)
#endif /* GCC.  */

#endif

/** @} */

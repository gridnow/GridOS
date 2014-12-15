/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*/

/**
*  @defgroup Stddef
*  @ingroup 标准C库编程接口
*
*  标准定义
*  @{
*/
#ifndef STDDEF_H
#define STDDEF_H

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})

#ifndef min
#define min(x, y) ({				\
		typeof(x) _min1 = (x);			\
		typeof(y) _min2 = (y);			\
		(void) (&_min1 == &_min2);		\
		_min1 < _min2 ? _min1 : _min2; })
#endif
#ifndef max
#define max(x, y) ({				\
		typeof(x) _max1 = (x);			\
		typeof(y) _max2 = (y);			\
		(void) (&_max1 == &_max2);		\
		_max1 > _max2 ? _max1 : _max2; })
#endif

#define abs(x) ({								\
		long ret;								\
		if (sizeof(x) == sizeof(long)) {		\
			long __x = (x);						\
			ret = (__x < 0) ? -__x : __x;		\
		} else {								\
			int __x = (x);						\
			ret = (__x < 0) ? -__x : __x;		\
		}										\
		ret;									\
	})

#define abs64(x) ({					\
		s64 __x = (x);				\
		(__x < 0) ? -__x : __x;		\
	})

/*  获取对其的长度 */
#define ALIGN(x, a)				__ALIGN__((x), (a))
#define __ALIGN__(x, a)			__ALIGN__MASK(x, (typeof(x))(a) - 1)
#define __ALIGN__MASK(x, mask)	(((x) + (mask)) & ~(mask))
#define PTR_ALIGN(p, a)		((typeof(p))ALIGN((unsigned long)(p), (a)))

/* for ptrdiff_t */
#define __PTRDIFF_TYPE__ long int
typedef __PTRDIFF_TYPE__ ptrdiff_t;

#endif

/** @} */


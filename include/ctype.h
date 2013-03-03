/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*/

/**
*  @defgroup Ctype
*  @ingroup 标准C库编程接口
*
*
*  @{
*/
#ifndef _CTYPE_H_
#define _CTYPE_H_

#define isupper(c)	('A' <= (c) && (c) <= 'Z')
#define islower(c)	('a' <= (c) && (c) <= 'z')
#define toupper(c)	(islower(c) ? ((c) - 'a' + 'A') : (c))
#define tolower(c)	(isupper(c) ? ((c) - 'A' + 'a') : (c))
#define isdigit(c)	('0' <= (c) && (c) <= '9')
#define isalnum(c)	(('0' <= (c) && (c) <= '9') \
	|| ('a' <= (c) && (c) <= 'z') \
	|| ('A' <= (c) && (c) <= 'Z'))
#define isxdigit(c)	(('0' <= (c) && (c) <= '9') \
	|| ('a' <= (c) && (c) <= 'f') \
	|| ('A' <= (c) && (c) <= 'F'))


#endif
/** @} */

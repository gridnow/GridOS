/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*/

/**
*  @defgroup MemoryMapping
*  @ingroup ��׼C���̽ӿ�
*
*  Posix memory map
*
*  @{
*/

#ifndef POSIX_MMAN_H
#define POSIX_MMAN_H

#include <types.h>
#define PROT_READ		0x1				/* Page can be read */
#define PROT_WRITE		0x2				/* Page can be written */
#define PROT_EXEC		0x4				/* Page can be executed */
#define PROT_NONE		0x0				/* Page can not be accessed */

#define MAP_SHARED		0x01			/* Share changes. */
#define MAP_PRIVATE		0x02			/* Changes are private. */
#define MAP_FIXED		0x10			/* Interpret addr exactly. */
#define MAP_ANONYMOUS	0x20			/* Don't use a file. */
#define MAP_ANON		MAP_ANONYMOUS
#define MAP_FAILED		((void *) -1)

/**
	@brief ӳ���ļ�����Я��64λƫ��

	@param[in] addr		ָ��ӳ��������ʼ��ַ�������NULL����ϵͳ�Զ�����
	@param[in] len		���õ�ַ���ȣ�ϵͳ�Զ�������ҳ��С����
	@param[in] prot		����ӳ�����ı�����ʽ
	@param[in] flags	����ӳ������ӳ�䷽ʽ
	@param[in] fd		ָ��Ҫ��ӳ���ļ����ļ�������
	@param[in] offset	�û��Զ���ƫ��

	@return �ɹ�����ӳ�����ĵ�ַ�����򷵻�MAP_FAILED

*/
void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);

#endif

/** @} */

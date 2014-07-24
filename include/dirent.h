/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*/

/**
*  @defgroup DIRENT
*  @ingroup ��׼C���̽ӿ�
*
*  ��׼Ŀ¼�����ӿڣ�ͷ�ļ���dirent.h
*
*  @{
*/

#ifndef _DIRENT_H
#define _DIRENT_H

#include <types.h>

BEGIN_C_DECLS;

/* GLIBC �������������ȣ���������ô��? */
#define NAME_MAX 256

struct dirent64
{
    u64					d_ino;									/* δʹ�ã���Ϊ����Linux */
    u64					d_off;									/* δʹ�ã���Ϊ����Linux */
    unsigned short int	d_reclen;								/* δʹ�ã���Ϊ����Linux */
    unsigned char		d_type;									/* �ļ����ͣ�Ŀ¼���ļ� */
    char				d_name[NAME_MAX];						/* �ļ��� */
};

struct dirent
{
    u32					d_ino;									/* δʹ�ã���Ϊ����Linux */
    u32					d_off;									/* δʹ�ã���Ϊ����Linux */
    unsigned short int	d_reclen;								/* δʹ�ã���Ϊ����Linux */
    unsigned char		d_type;									/* �ļ����ͣ�Ŀ¼���ļ� */
    char				d_name[NAME_MAX];						/* �ļ��� */ 
};

struct __dirstream;
typedef struct __dirstream DIR;

/**
	@brief ��Ŀ¼

	@param[in]	dirname	Ŀ¼��
	
	@return �ɹ�����DIRָ�룬ʧ���򷵻�NULL��������errnoֵ��
*/
DIR *opendir(const char *dirname);

/**
	@brief �ر�Ŀ¼
	
	@param[in]	dirp	Ŀ¼������DIR

	@return �ɹ�����0��ʧ���򷵻�-1��������errnoֵ��
*/
int closedir(DIR *dirp);

/**
	@brief Ŀ¼���ȡ

	@param[in]	dirp	Ŀ¼������DIR
	
	@return �ɹ�����Ŀ¼��������ָ�룬ʧ�ܻ�ﵽĿ¼��β����NULL��������errnoֵ��
*/
struct dirent *readdir(DIR *dirp);

/**
	@brief 64λ�汾Ŀ¼���ȡ

	����ͬ32λ�汾readdirһ����

	@param[in]	dirp	Ŀ¼������DIR
	
	@return �ɹ�����Ŀ¼��������ָ�룬ʧ�ܻ�ﵽĿ¼��β����NULL��������errnoֵ��
*/
struct dirent64 *readdir64(DIR *dirp);

/**
	@brief ���ö�ȡλ�õ�Ŀ¼��ʼλ��

	@param[in]	dirp	Ŀ¼������DIR

	@return �޷���ֵ��
*/
void rewinddir(DIR *dirp);

/**
	@brief	���ݹ���ģʽɨ��Ŀ¼�µķ�Ŀ¼���ļ�

	���ؽ���Ǿ���compare�����Ŀ¼���б�

	@param[in]	dir		Ŀ¼��
	@param[in]	namlist ���ڴ��Ŀ¼���б�
	@param[in]	filter	���˺���ָ��
				���δ����filter��������filterΪNULL��������Ŀ¼�ֱ�Ӵ�ŵ�Ŀ¼���б�
				���δ����filter������filter����0��Ŀ¼���޷��������ģʽ�����ܴ�ŵ�Ŀ¼���б�

	@param[in]	compare ������ָ��
				���δ����compare�����򱣳�Ŀ¼�б�Ĭ������

	@return	�ɹ�����ƥ�����ģʽ��Ŀ¼�������ʧ���򷵻�-1��������errnoֵ��
*/
int scandir(const char *dirname, 
			struct dirent ***namelist,
			int (*filter)(const struct dirent *),
			int (*compare)(const struct dirent **, const struct dirent **));

END_C_DECLS;

#endif
/** @} */


/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*/

/**
*  @defgroup dlfcn
*  @ingroup ��׼C���̽ӿ�
*
*  posix ���ݵĶ�̬��ӿ�
*
*  @{
*/
#ifndef POSIX_DLFCN_H
#define POSIX_DLFCN_H

#include <compiler.h>
#include <types.h>

#define RTLD_LAZY			0x00001	/* Lazy function call binding.  */
#define RTLD_NOW			0x00002	/* Immediate function call binding.  */
#define	RTLD_BINDING_MASK   0x3	/* Mask of binding time value.  */
#define RTLD_NOLOAD			0x00004	/* Do not load the object.  */

#define RTLD_GLOBAL			0x00100
#define RTLD_LOCAL			0
#define RTLD_NODELETE		0x01000
typedef struct
{
	const char *dli_fname;	/* File name of defining object.  */
	void *dli_fbase;			/* Load address of that object.  */
	const char *dli_sname;	/* Name of nearest symbol.  */
	void *dli_saddr;			/* Exact value of nearest symbol.  */
} Dl_info;

/**
	@brief �򿪹���̬��

	@param[in] file ��Ч��̬����, NULL ��ʾ��ȡȫ�ֶ�����
	@param[in] mode ��̬���ģʽ
	
	@return 
		NULL ʧ�ܣ���ȷΪ��̬�����ָ��

	@note
		Posix: If the value of file is NULL, dlopen() shall provide a handle on a global symbol object.
*/
void *dlopen(const char *file, int mode);

/**	
	@brief �رն�̬
	
	��ͨ��dlopen�򿪵Ķ�̬��رգ�����Ҳ�����ٷ��ʸö�̬�⡣

	@param[in] handle ��̬�����
	@return
		0 on success ,other is error	
*/
int dlclose(void *handle);

/**
	@brief ��ö�̬��ķ��ŵ�ַ

	�ҵ�ָ���������ڽ����еĵ�ַ��

	@param[in] handle ��̬�����
	@param[in] name ������
	@return 
		���ŵ�ַ��NULL����ʧ��
*/
void *dlsym(void *__restrict handle, __const char *__restrict name);

/**
	@brief ��һ���а汾��ŵķ��ŵ�ַ
	
	�ҵ����汾��Ϣ�ķ������ڽ����еĵ�ַ��

	@param[in] handle ��̬�����
	@param[in] name ������
	@param[in] version �汾��

	@return 
		���ŵ�ַ��NULL����ʧ��
*/
void *dlvsym(void *__restrict handle, __const char *__restrict name,
			  __const char *__restrict version);

/**
	@brief ���ض�̬��ģ��Ĵ�����Ϣ

	When any of the above functions fails, call this function to return a string describing the error.  
	Each call resets the error string so that a following call returns null. 

	@return
		������Ϣ���ַ���������NULL	
*/
char *dlerror (void);

/**
	@brief ͨ����ַ�ҵ�����
	
	ͨ��һ����ַ�ҵ�������Ӧ����ķ������֣����ҷ��ص��û�ָ����info��������

	@param[in] address Ҫ�ҵĵ�ַ
	@param[in] info ���ؽ������ЧDl_info������

	@return
		0 if the specified address can't be matched, or nonzero if it could be matched.
		If success,the info structure will be filled.
		If failed,the info->sname and info->sname will be set to NULL
*/
int dladdr(void * address, Dl_info * info);

/********************************************************
				Extended
*********************************************************/
void dlcall_posix_entry(void *entry, int argc, char *argv[]);
void *dlentry(void *__restrict handle);
/* 
	@brief ��ȡsection �������ַ
*/
void *dlsection_vaddr(void *handle, const char *section_name, size_t *size);

#endif
/** @} */

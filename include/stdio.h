/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*/

/**
*  @defgroup STDIO
*  @ingroup ��׼C���̽ӿ�
*
*  ��׼������������ӿڣ�ͷ�ļ���stdio.h
*
*  @{
*/
#ifndef _STDIO_H
#define _STDIO_H

#include <types.h>
#include <stdarg.h>

BEGIN_C_DECLS;

typedef void* FILE;
extern FILE stderr;
extern FILE stdout;
extern FILE stdin;

/**
	@brief �Ѹ�ʽ��������д��ĳ���ַ���������

	�ִ���ʽ�������Ҫ�����ǰѸ�ʽ��������д��ĳ���ַ����С�
	�ú����Ǹ���κ�����ʹ��ʱ���������⣬Ҫע�⻺�����ĳ����Ƿ����������кϳɵ��ַ���

	@param[in] buffer ָ����д����ַ�����ַ
	@param[in] format ָ����ڴ������˸�ʽ�ַ���

	@note
		Ҫ�ϳɵ��ַ���������������С��ᵼ���ڴ�������ɿ�����snprintf��
	@return
		�ϳɵ��ַ����ֽڳ��ȣ���������������
*/
int sprintf (char *__restrict buffer, __const char *__restrict format, ...);

/**
	@brief �Ѹ�ʽ��������д��ĳ���ַ���������

	����ͬsprintf�����Ǹýӿڸ���ȫ���û���ָ���ϳɵ��ַ������ߴ磬��֤�������������

	@param[in] buffer ָ����д����ַ�����ַ
	@param[in] size Ŀ�껺���������ߴ�
	@param[in] format ָ����ڴ������˸�ʽ�ַ���

	@return
		�ϳɵ��ַ����ֽڳ��ȣ���������������������صĳ��ȱȸ����ĳߴ糤����ôĿ�껺�����е����ݽ����ü���
*/	
int snprintf(char *__restrict buffer, __const size_t size, const char *format, ...);

/**
	@brief ͨ����׼����豸���һ������

	��׼�⺯��vprintf������printf�������ƣ�����ͬ���ǣ�����һ������ȡ���˱䳤�������Ҵ˲���ͨ������va_start����г�ʼ����
	
	@param[in] format ָ����ڴ������˸�ʽ�ַ���
	@param[in] arg ��������Ŀɱ��б�

	@return
		������ַ����ֽڳ��ȡ�
*/
int vprintf(const char *format, va_list arg);

/**
	@brief �Ѹ�ʽ��������д��ĳ���ļ�

	���ܴ�����sprintfһֱ�����������Ŀ�겻����ĳ��һ������������һ���ļ���

	@param[in] file ָ����д����ļ�����
	@param[in] format ָ����ڴ������˸�ʽ�ַ���

	@return
		������ַ����ֽڳ��ȡ�
*/
int	fprintf(FILE *file, const char *format, ...);

/**
	@brief ͨ����׼����豸���һ������

	ת������ʽ����д Argument ��������׼�����

	@param[in] format ָ����ڴ������˸�ʽ�ַ���

	@return
		������ַ����ֽڳ��ȡ�
*/
int printf(const char *format, ...);

/**
	@brief ͨ����׼����豸���һ���ַ���

	@param[in] str ָ����ڴ��������ַ���

	@return
		������ַ����ֽڳ��ȡ�
*/
int puts(const char *str);

/**
	@brief ͨ����׼����豸���һ���ַ�

	@param[in] ch Ҫ������ַ�����

	@return
		������ַ��ֽڳ��ȡ�
*/
int putchar(int ch);

/**
	@brief �Ѹ�ʽ��������д��ĳ���ַ���������

	�ִ���ʽ�������Ҫ�����ǰѸ�ʽ��������д��ĳ���ַ����С�
	�ú����Ǹ���κ�����ʹ��ʱ���������⣬Ҫע�⻺�����ĳ����Ƿ����������кϳɵ��ַ���

	@param[in] buffer ָ����д����ַ�����ַ
	@param[in] format ָ����ڴ������˸�ʽ�ַ���
	@param[in] arg ��������Ŀɱ��б�

	@note
		�ú������׳����ڴ�������������ʹ��vsnprintf��
	@return
		��������·��������ִ��ĳ���(��ȥ\0)������������ظ�ֵ��
*/
int vsprintf (char *__restrict buffer, __const char *__restrict format, va_list arg);

/**
	@brief �Ѹ�ʽ��������д��ĳ���ַ���������

	�ִ���ʽ�������Ҫ�����ǰѸ�ʽ��������д��ĳ���ַ����С�
	�ú����Ǹ���κ�����ʹ��ʱ���������⣬Ҫע�⻺�����ĳ����Ƿ����������кϳɵ��ַ���

	@param[in] buffer ָ����д����ַ�����ַ
	@param[in] format ָ����ڴ������˸�ʽ�ַ���
	@param[in] size Ҫ����������ֽ���
	@param[in] arg ��������Ŀɱ��б�

	@return
		��������·��������ִ��ĳ���(��ȥ\0)������������ظ�ֵ��
*/
int vsnprintf(char *__restrict buffer, size_t size, const char *__restrict format, va_list args);

/**
	@brief	��һ���ļ�

	@param[in]	name	�ļ���
	@param[in]	mode	�������ͻ�򿪷�ʽ��Ŀǰ֧�ֵ��������£�
			<ul>
				<li>r	��ֻ���ļ������ļ�������ڣ�
				<li>r+	�򿪿ɶ�д�ļ������ļ�������ڣ�
				<li>w	��ֻд�ļ������ļ�����������ļ����ݣ����ļ�����Ϊ0��
						���ļ��������򴴽��ļ���
				<li>w+	�򿪿ɶ�д�ļ������ļ�����������ļ����ݣ����ļ�����Ϊ0��
						���ļ��������򴴽��ļ���
				<li>a	���ӷ�ʽ��ֻд�ļ������ļ�������д������ֻ��׷�����ļ�β���ļ�ԭ�����޷��޸ģ�
						���ļ��������򴴽��ļ���
				<li>a+	���ӷ�ʽ�򿪿ɶ�д�ļ������ļ�������д������ֻ��׷�����ļ�β���ļ�ԭ�����޷��޸ģ�
						���ļ��������򴴽��ļ���	
				<li>b	��������
			</ul>

	@return	�ļ��򿪳ɹ�����ִ�и��ļ����ļ�ָ�룬ʧ���򷵻�NULL��������errnoֵ��
*/
FILE *fopen(const char *name, const char *mode);

/**
	@brief	��ȡ�ļ�����

	���ļ��ж�ȡnmemb������Ԫ�أ�ÿ������Ԫ�ذ���size�ֽڡ�

	@param[in]	ptr		���ڽ����ļ����ݵ��ڴ��ַ����С������size*nmemb�ֽ�
	@param[in]	size	����Ԫ�ش�С����λ���ֽ�
	@param[in]	nmemb	����Ԫ�ظ���
	@param[in]	file	�ļ�

	@return	��ȡ�ɹ�����ʵ�ʶ�ȡ������Ԫ�ظ�����ʧ���򷵻�0��������errnoֵ��
*/
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *file);

/**
	@brief	���ļ�д������

	���ļ���д��nmemb������Ԫ�أ�ÿ������Ԫ�ذ���size�ֽڡ�
	д����Ϊ���ļ��򿪷�ʽֱ����ء�

	@param[in]	ptr		���ڴ���������ݵ��ڴ��ַ
	@param[in]	size	����Ԫ�ش�С����λ���ֽ�
	@param[in]	nmemb	����Ԫ�ظ���
	@param[in]	file	�ļ�

	@return	д��ɹ�����ʵ��д�������Ԫ�ظ�����ʧ���򷵻�0��������errnoֵ��
*/
size_t fwrite(void *ptr, size_t size, size_t nmemb, FILE *file);

/**
	@brief	�ر��ļ�

	@param[in]	file	�ļ�

	@return	�رճɹ�����0�����򷵻�-1��
*/
int fclose(FILE *file);

/**
	@brief	��λ�ļ���ȡλ��

	�ú��������ļ��ڲ�������λ�ã��Ӷ����÷����ļ���λ��

	@param[in]	file	�ļ�
	@param[in]	offset	����ļ�����λ�õ�ƫ����
	@param[in]	whence	����λ������ģʽ���ļ�ͷSEEK_SET���ļ�βSEEK_END���ļ���ǰλ��SEEK_CUR

	@return	��λ�ɹ�����0��ʧ���򷵻�-1��������errnoֵ��
*/
int fseek(FILE *file, long offset, int whence);

/**
	@brief	��ȡ�ļ���ǰ����λ��

	@param[in]	file	�ļ�
	
	@return	�ɹ������ļ���ǰ����λ�ã�ʧ���򷵻�-1��������errnoֵ��
*/
long ftell(FILE *file);

/**
	@brief ˢ��һ�����Ļ���

	ǿ��ˢ���ļ����û�̬�Ļ������ݣ����û��ָ���������ĸ���(��ڲ�����NULL)����ˢ�����д򿪵��ļ�����

	@param[in]	stream	�ļ�,NULL ��ʾ�����д򿪵��ļ�������

	@return �ɹ�����0������EOF ��ʾ����
*/	
int fflush(FILE *stream);


/**
	@brief �ӱ�׼�����ϻ�ȡһ������

	һ��һ������1���ֽڣ����û����¼���ʱ�ú�������һ���ֽڣ�����
	posix ������ĳЩ�������緽������Ƕ��ֽڵģ���˵��ú����õ���
	��ֵ������ֵ��ʱ���б�Ҫ�ٴε��øú����õ�ʣ�µļ�ֵ�������õ�
	һ��������

	@note
		�����ֵ�μ�POSIX��׼��

	@return
		��ֵ�����û�а������߳̽�������״̬��
*/
int getch();

/**
	@brief fseekλ��ȡֵ
*/
#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2

/*
	For debug only
*/
#define PRINT printf
#include "ddk/debug.h"

END_C_DECLS;

#endif

/** @} */


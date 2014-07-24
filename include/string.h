/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*/

/**
*  @defgroup STRING
*  @ingroup ��׼C���̽ӿ�
*
*  ��׼�ַ��������ӿڣ�ͷ�ļ���string.h 
*
*  @{
*/
#ifndef _STRING_H
#define _STRING_H

#include <types.h>

BEGIN_C_DECLS;
/**
	@brief ��һ�������е��������Ϊ�ض�����

	��һ���ڴ�������ĳ��������ֵ�����ǶԽϴ�Ľṹ�������������������һ����췽����

	@param[in] dst Ŀ�껺������ַ���û��㱣֤�����㹻��size�ֽڿռ�
	@param[in] ch Ҫ�����ַ�����Ȼ��int�͵ģ�����ֻ�ܵ�char���͵�ʹ�ã�����ҵ�ڹ涨
	@param[in] size Ҫд������ַ�

	@return
		dst�ĵ�ַ��
*/
void *memset(void *dst, int ch, unsigned long size);

/**
	@brief ����һ�������������ݵ�����һ��������

	��Դsrc��ָ���ڴ��ַ����ʼλ�ÿ�ʼ����len���ֽڵ�Ŀ��dst��ָ���ڴ��ַ����ʼλ���У�
	һ�������ڴ濽�����������������ڴ�����Ʋ����ĳ����������ַ���������һ�����������ڴ��е�����
	��0����ֹ������������size���������Ƴ��ȡ�
	���⣬dst �� src�ڴ�ռ䲻���ص���

	@param[in] dst Ŀ�껺�������û��㱣֤���㹻���ڴ汣��src��ָ�������
	@param[in] src Դ������
	@param[in] size Ҫ�������ַ����ֽڳ���

	@return
		Ŀ�괫��ȥ��ַdst��
*/
void *memcpy(void *dst, const void *src, unsigned long size);

/**
	@brief �����ַ���

	�Ѵ�src��ַ��ʼ�Һ���NULL���������ַ�����ֵ����dest��ʼ�ĵ�ַ�ռ䣬
	src��dest��ָ�ڴ����򲻿����ص���dest�������㹻�Ŀռ�������src���ַ�����
	�ú�����ֹ���������ڿ������ַ����Ĺ�����src�ַ��������˽�����'\0'��

	@param[in] dest Ŀ�껺������ַ���û��㱣֤���㹻���ڴ汣��src��ָ����ַ���
	@param[in] src  Դ��������ַ

	@return Ŀ�괮�ĵ�ַ��
*/
char *strcpy(char *__restrict dest, __const char *__restrict src);

/**
	@brief �������ָ�����ȵ��ַ���

	�Ѵ�src��ַ��ʼ�Һ���NULL���������ַ�����ֵ����dest��ʼ�ĵ�ַ�ռ䣬
	src��dest��ָ�ڴ����򲻿����ص���dest�������㹻�Ŀռ�������src���ַ�����
	�ú�����ֹ�����������Ѿ��ﵽ����len�򿽱����ַ����Ĺ�����src�ַ��������˽�����'\0'��

	@param[in] dest Ŀ�껺������ַ���û��㱣֤���㹻���ڴ汣��src��ָ����ַ���
	@param[in] src  Դ��������ַ

	@return Ŀ�괮�ĵ�ַ��
*/
char *strncpy (char *dest, const char *src, int len);

/**
	@brief ���ַ���ͷ����β������һ���ַ�

	�����ַ������״γ���ĳ�ַ���λ�ã���������������

	@param[in] str Ҫ�������ַ���
	@param[in] ch  Ҫ���ҵ��ַ�

	@return �����ַ��״γ��ֵ�λ��ָ�룬����ַ����в�����Ҫ�������ַ��򷵻�NULL��
*/
char *strchr(const char *str, int ch);

char *strrchr(const char *str, int ch);

/**
	@brief �Ƚ������ַ����������Ƿ�һ��
	
	�����ַ���������������ַ���ȣ���ASCIIֵ��С��Ƚϣ�����Сд���У���ֱ�����ֲ�ͬ���ַ�����'\0'Ϊֹ��
	
	@param[in] s1 ��һ���ַ���
	@param[in] s2 �ڶ����ַ���

	@return
		��s1<s2ʱ������ֵ<0����s1=s2ʱ������ֵ=0����s1>s2ʱ������ֵ>0��
*/
int strcmp(__const char *s1, __const char *s2);

/**
	@brief ����󳤶��ڱȽ������ַ����������Ƿ�һ��
	
	�����ַ���������������ַ���ȣ���ASCIIֵ��С��Ƚϣ�����Сд���У���ֱ�����ֲ�ͬ���ַ�����'\0'Ϊֹ���߳�����ָ������󳤶ȡ�
	
	@param[in] s1 ��һ���ַ���
	@param[in] s2 �ڶ����ַ���
	@param[in] count Ҫ�Ƚϵ���󳤶ȡ�����ַ����ĳ���û����󳤶��������ĳ�����ô���ַ����������󳤶�ΪҪ�Ƚϵĳ������ޡ������ֵΪ0����ô����ֱ�ӷ���0��

	@return
		��s1<s2ʱ������ֵ<0����s1=s2ʱ������ֵ=0����s1>s2ʱ������ֵ>0��
*/
int strncmp(const char *s1, const char *s2, size_t count);

/**
	@brief �����ַ����ĳ���

	��һ���ַ����׵�ַ��ʼɨ�裬ֱ�������ַ���������'\0'Ϊֹ��

	@param[in] str Ҫͳ�Ƴ��ȵ��ַ����׵�ַ

	@return
		�ַ����ַ����ȣ�������'\0'���ڡ�
*/
unsigned long strlen(const char * str);

/**
	@brief �����ַ����ĳ���

	��һ���ַ����׵�ַ��ʼɨ�裬ֱ�������ַ���������'\0'Ϊֹ�����߱ȶ��ֽ����ﵽ��maxlen�����ޡ�

	@param[in] str Ҫͳ�Ƴ��ȵ��ַ����׵�ַ
	@param[in] str Ҫͳ�Ƶ���󳤶�

	@return
		�ַ����ַ����ȣ�������'\0'���ڡ������󳤶ȳ������ַ�������ĳ��ȣ���ô�����ַ�����ȫ���ȣ����򷵻���󳤶ȡ�
*/
size_t strnlen(const char *s, size_t maxlen);

/**
	@brief �Ƚ��ڴ�����������Ƿ���ͬ

	һ�������ƱȽϺ�����һ���ֽ�һ���ֽڵرȽ������������������Ƿ���ͬ��

	@param[in] cs Դ��������ַ
	@param[in] ct Ŀ�껺������ַ
	@param[in] count Ҫ�Ƚϵ��ַ���

	@return 
		��cs<ctʱ������ֵ<0����cs=ctʱ������ֵ=0����cs>ctʱ������ֵ>0��
*/
int memcmp(const void *cs, const void *ct, size_t count);

/**
	@brief Append one NULL-terminated string to another
*/
char *strcat(char *dst, const char *src);

END_C_DECLS;

#endif

/** @} */


/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Jerry,Wuxin
 *   General i2p
 */


#ifndef COMMON_I2P_H
#define COMMON_I2P_H

#include <types.h>
#include <stddef.h>
#include <limits.h>

#define MAX_COUNT_PER_ARRAY			1024
#define MAX_ARRAY_SIZE				(MAX_COUNT_PER_ARRAY * sizeof(void *))
#define COMMON_I2P_ALLOC_ERROR		(-1UL)

typedef unsigned long i2p_handle;
typedef	void  (*i2p_free)(void *p);
typedef	void *(*i2p_malloc)(size_t size);
struct i2p;

/**
	@brief ����һ��i2pת������

	�û�ͨ��i2pת������ʵ��index <-> pointer֮���˫��ת������

	@param[in] malloc_handle i2p��������Ҫ�õ����ڴ���亯��
	@param[in] free_handle i2p��������Ҫ�õ����ڴ���պ���

	@return �ɹ������½���i2p����ָ�룬ʧ����Ϊ NULL;
*/
struct i2p *i2p_create(i2p_malloc malloc_handle, i2p_free free_handle);

/**
	@brief ɾ��i2pת������
	
	@param[in] i2pt ���ͷŵ�i2pt����
*/
void i2p_delete(struct i2p *i2pt);

/**
	@brief ��������

	��i2pt�����и�ptrָ�����һ��δ�õ�����

	@param[in] i2pt ��������i2p����
	@param[in] ptr  �����ptrָ�� 
	@return �ɹ�����������ʧ����Ϊ COMMON_I2P_ALLOC_ERROR;
*/
i2p_handle i2p_alloc(struct i2p *i2pt, void *ptr);

/**
	@brief �ͷ�i2p�����е�һ������

	����һ��i2p���󣬸���size��С����ʼ��i2p_node�е�bitmap��

	@param[in] i2pt ����Ĵ�������i2p��ָ��
	@param[in] index ����Ĵ��ͷŵ�������
	@return �ɹ�����true��ʧ����Ϊfalse;
*/
bool i2p_dealloc(struct i2p *i2pt, i2p_handle index);

/**
	@brief ͨ�����������Ҷ�Ӧ��ָ�롣

	��i2pģ���в��Ҷ�Ӧ���������ʹ�����ڲ�ѯʱ���������I2P ��ѯ����Ͳ���ɾ����ͻ

	@param[in] i2pt ����Ĵ����ҵ�i2p��ָ��
	@param[in] index ����Ĵ����ҵ�������
	@return  �ɹ������ҵ��ĵ�ַָ�룬ʧ����Ϊ NULL;
*/
void *i2p_find(struct i2p *i2pt, i2p_handle index);

/**
	@brief �������о��

	�þ���������еľ�����Ա�������ÿ���������action�������д���
	
	action��������2������processָ���handle

	@param[in] i2pt ����Ĵ����ҵ�i2p����ָ��
	@param[in] action ����Ĵ�����ָ��
	@param[in] process ���ݸ��ص�������processָ�룬ָʾ��ǰ����
	@return  �ɹ������ľ����;
*/
unsigned long i2p_loop(struct i2p *i2p_list, void (*action)(void *process, unsigned long handle_index), void *process);

#endif


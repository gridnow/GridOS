/**
*  @defgroup memalloc
*  @ingroup common_libs
*
*  �ڴ����ģ��
*
*  @{
*/

#ifndef COMMON_LIBS_MEMALLOC
#define COMMON_LIBS_MEMALLOC

/**
	@brief ��һ������ڴ��ϴ���������
	
	�����ķ������������ڲ�����С���ڴ�����

	@param[in] big_block �����ʼ��ַ�����������еĹ������ݽṹ���û�������ڴ�鶼������ڸÿ��ַ��
	@param[in] size ����ַ����
	
	@return bool
*/
bool memalloc_init_allocation(void * big_block, int size);

/**
	@brief ����һ���ڴ�

	�ڳ�ʼ�����һ���ڴ���Ϸ���ָ����С���ڴ��

	@param[in] big_block ���ڴ�飬������ͨ����ʼ���ӿڳ�ʼ�����ģ�����������ȷ�����
	@param[in] size Ҫ������ڴ��ߴ�

	@return ʧ�ܷ���NULL���ɹ����ؿ��õ��ڴ���ַ
*/
void * memalloc(void * big_block, int size);

/**
	@brief �ͷ�һ���ڴ�

	�ڳ�ʼ�����һ���ڴ�����ͷ�ָ�����ڴ��

	@param[in] big_block ���ڴ�飬������ͨ����ʼ���ӿڳ�ʼ�����ģ�����������ȷ�����
	@param[in] p Ҫ���ͷŵ��ڴ���ַ�������ָ����NULL��ֱ�ӷ���NULL�����p��һ�����ͷŹ���ָ�룬ֱ�ӷ���NULL����������������ֵ��������ȷ�����

	@return �ɹ�����p, ʧ�ܷ���NULL�������ǲ�ȷ�����
*/
void * memfree(void * big_block, void * p);

#endif
/** @} */

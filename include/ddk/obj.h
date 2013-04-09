/**
*  @defgroup Object
*  @ingroup DDK
*
*  �������������豸����������Ľӿ�
*  @{
*/

#ifndef _DDK_OBJ_H_
#define _DDK_OBJ_H_

#include <ddk/types.h>

typedef void * device_t;
typedef void * driver_t;

/**
	@brief ���豸������صĲ������������豸�������ṩ 
*/
struct do_device_type
{
	const char * name;
	int size_of_device;
	
	/**
		@brief �������豸��������ƥ�䷽��
	*/
	bool (*match)(device_t device, driver_t driver);				/**< ����ƥ��ӿڣ��豸����������ÿ����ע����豸����������ƥ�䣬���Ǿ����ƥ�䷽�� */

	bool (*delete)(device_t device);								/**< �����豸��ɾ�����������������������豸���ü�����Ϊ0������µ��øýӿڡ�
																		����豸�����ٶ������������ã���ô����ϵͳ����ң�������һ��������������д����׳�������
																	*/
	/**
		@brief ̽��ĳ���豸�Ƿ��Ǹ����͵�
	*/
	bool (*probe)(device_t device);

	/* Not filled by driver, reserved for manager (cl_object_type) */
	unsigned long dummy;
	unsigned char dummy_byte[256];	
};

/**
	@brief ע���豸�����豸��������

	���豸�����������һ�����豸����ƥ����ڵ������������ƥ��ɹ���������������ʹ�ø�Ӳ����

	@param[in] device Ҫע������豸
	@param[in] parent ���豸�ĸ��豸	

	@return
		�ɹ�����true��ʧ�ܷ���false��������豸����ע�������ô����false

	@note
		1,���parent���ڣ���ôparent��������һ�����ã���ˣ�Ҫɾ��parent�ȵ�ɾ��device
		2,type�����������������ṩ���������豸��������Ϣ�����Ͳ�������
*/
bool do_register_device(device_t device, device_t parent);

/**
	@brief ���豸�������г���һ���豸��ע����Ϣ
*/
void do_unregister_device(void * device);

/**
	@brief ע���������豸��������

	���豸�����������һ���µ���������������Ϣ�������ø�������ƥ�䷽��ƥ���豸��������ע����Щû�жԽ��������豸��
	���������ƥ�䷽���ܳɹ�ʶ��Ӳ���������������豸��������������ϵ��

	@param[in] driver �������������壬��ddk_pci_driver
	@param[in] type ��������������������豸���ͣ����һ��ͨ�������ض����ߵĽӿڻ�ȡ

	@return
		true ע��ɹ���falseע��ʧ��
*/
bool do_register_driver(void * driver, struct do_device_type * type);

/**
	@brief �����豸���������
*/
bool do_set_device_name(device_t device, const char *fmt, ...);

/**
	@brief ��ȡ�豸���������
*/
char *do_get_device_name(device_t device);

/**
	@brief �ͷ��豸������ָ��
*/
device_t do_put_device(device_t device);

/**
	@brief �����豸����ĵ�ַ
*/
void *do_alloc_raw(struct do_device_type *type);

/**
	@brief ������Ҫ����һ������ָ��
*/
device_t do_get_device(device_t device);

/**
	@brief �����ض����͵��豸

	@param[in] type Ҫ�������豸����
	@param[in] start Ҫ��ʼ�������豸�����ΪNULL����Ӹ����͵ĵ�һ���豸��ʼ����
	@param[in] data ��ƥ�亯���Ĳ����������Զ���
	@param[in] match ƥ��ص��������������true,��ֹͣ����

	@return	
		����ƥ��ɹ����豸,NULL��ʾ��ֹ��ʧ��
*/
device_t do_find_device(struct do_device_type * type, device_t start, void * data, bool (*match)(device_t dev, void *data));


//@param[in] type ���豸������������Ϣ�͸������豸�Ĳ�������
#endif

/** @} */

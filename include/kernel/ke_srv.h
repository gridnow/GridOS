/**
*  @defgroup kernel_api
*
*
*  @{
*/
/** @} */

/**
	@defgroup SrvManager
	@ingroup kernel_api
	
	�ں˱�̽ӿ� ϵͳ����

	@{
*/
#ifndef KE_SRV_H
#define KE_SRV_H

#include <types.h>

#define KE_SRV_MAX 16
#define KE_SRV_GET_FID(ID)				((ID) & 0xffffff)
#define KE_SRV_GET_CLASZ(ID)			((unsigned)(ID) >> 24)
#define KE_SRV_MAKE_REQ_NUM(BASE, ID)	((((BASE) & (KE_SRV_MAX - 1)) << 24) | KE_SRV_GET_FID(ID))

typedef unsigned long ke_handle;
typedef unsigned long KERNEL_STATUS;
#define KE_INVALID_HANDLE (-1UL)
#define KE_SUCCESS			0

struct ke_srv_info
{
	const char *name;
	unsigned long service_id_base, service_id_end;
	void *request_enqueue;
};

/**
	@brief Common request package
*/
struct sysreq_common
{
	unsigned int req_id;
};

bool ke_srv_register(const struct ke_srv_info *info);

/**
	@brief ����Ͷ��󶼲���Ҫ�ˣ��ڽӿں�����ʹ��

	@note
		����һ���Ǳ�ת���������Ҿ��û�б�put ����
*/
void ke_handle_and_object_destory(ke_handle handle, void *kobject);

/**
	@brief Handler part
*/
void ke_handle_put(ke_handle handle, void *kobject);
void *ke_handle_translate(ke_handle handle);
bool ke_handle_delete(ke_handle handle);
ke_handle ke_handle_create(void *kobject);

int ke_srv_null_sysxcal(void *req);

#endif
/** @} */

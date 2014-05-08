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
	
	内核编程接口 系统调用

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
	@brief 句柄和对象都不想要了，在接口函数中使用

	@note
		对象一定是被转换过，并且句柄没有被put 过。
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

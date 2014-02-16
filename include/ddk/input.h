/**
*  @defgroup Input
*  @ingroup DDK
*
*  ��������������Ҫ���õ��ڴ�������ӿ�
*  @{
*/
#ifndef _DDK_INPUT_H_
#define _DDK_INPUT_H_

#include <compiler.h>
#include <types.h>
#include <list.h>
#include <kernel/ke_complete.h>
#include <kernel/ke_lock.h>
#include <kernel/ke_atomic.h>

/* ��Щ�����?? */
#define SESSION_STD_INPUT_PKG_L_ALT 	(1<<0)
#define SESSION_STD_INPUT_PKG_R_ALT 	(1<<1)
#define SESSION_STD_INPUT_PKG_L_CTL 	(1<<2)
#define SESSION_STD_INPUT_PKG_R_CTL 	(1<<3)
#define SESSION_STD_INPUT_PKG_KEY_DOWN	(1<<4)

/* �����豸���� */
#define IFI_DEV_STD_IN  (1<<0)
#define IFI_DEV_MSC_IN  (1<<1)

#define IFI_DEVICE_PKG_COUNT	32

/*
 * Compatible event types
 */
#define DDK_INPUT_EV_SYN			0x00
#define DDK_INPUT_EV_KEY			0x01
#define DDK_INPUT_EV_REL			0x02
#define DDK_INPUT_EV_ABS			0x03
#define DDK_INPUT_EV_MSC			0x04
#define DDK_INPUT_EV_SW				0x05
#define DDK_INPUT_EV_LED			0x11
#define DDK_INPUT_EV_SND			0x12
#define DDK_INPUT_EV_REP			0x14
#define DDK_INPUT_EV_FF				0x15
#define DDK_INPUT_EV_PWR			0x16
#define DDK_INPUT_EV_FF_STATUS		0x17
#define DDK_INPUT_EV_MAX			0x1f
#define DDK_INPUT_EV_CNT			(DDK_INPUT_EV_MAX+1)

/*�����豸����*/
#define IFI_DEV_STD_IN  (1<<0)
#define IFI_DEV_MSC_IN  (1<<1)

#define IFI_DEVICE_PKG_COUNT	32


/* ddk struct */
struct ddk_input_handle
{
	void *drv_handle;

	void (*event)(struct ddk_input_handle *handle, unsigned int event_type, unsigned int event_code, int value);
	void (*start)(struct ddk_input_handle *handle);
	void (*disconnect)(struct ddk_input_handle *handle);
};

/* �豸����ṹ */
struct ifi_package
{
	u8 flags;
	u8 code;
};

struct ifi_device;

/**
**�豸����������
**��Ҫ��read,write
**/
typedef struct ifi_dev_ops
{
	/* ���豸�����û��ռ�,����ʵ�ʵĶ�ȡ���� */
	int (*ifi_dev_read)(struct ifi_device * dev, struct ifi_package * data, int len);
	/* ���豸д������,����ʵ��д��ĳ��� */
	int (*ifi_dev_write)(struct ifi_device * dev, void * data, int len);
}ifi_dev_ops_t;


struct ifi_device
{
	struct ke_completion data_ready;
	struct ke_spinlock lock;
	struct list_head dev_list;
	/* �豸���� */
	int dev_type;
	
	/* �豸���������� */
	struct ifi_package ifi_buffer[IFI_DEVICE_PKG_COUNT];
	short store_pos, fetch_pos;
	struct ke_atomic free_count;

	int ext_code;//�����־λ�����ж϶๦�ܼ�

	struct ifi_dev_ops *dev_ops;
	/* �豸���뺯������,�ú������ж������ı����� */
	int (*ifi_input_stream)(struct ifi_device *dev, void *data, size_t len);
	/* �豸������� */
	int (*ifi_output_stream)(struct ifi_device *dev);
	/* �豸���ٺ��� */
	void (*ifi_dev_destroy)(struct ifi_device *dev);
};

/*
*	�����豸ʵ��
*/
DLLEXPORT struct ifi_device * get_ifi_dev_by_devtype(int type);

/**
	@brief ĳ�������豸��������������

	һ���жϴ������ڶ�ȡ����Ӳ�����ݺ����

	@note
		����ͬһ�����󲻿�����!
*/

DLLEXPORT int ifi_input_stream(int type, void * buf, size_t size);

/**
	@brief ��ȡ������

	@return
		�����ֽڳ���
*/
DLLEXPORT int ifi_read_input(void * input, int type);

/**
	@brief �豸����
*/
DLLEXPORT struct ifi_device * ifi_device_create(void);

/* An kernel special interface for debuging */
DLLEXPORT int ke_input_debug(unsigned int code);


#endif

/** @} */



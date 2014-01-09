/**
*  @defgroup Input
*  @ingroup DDK
*
*  定义了驱动程序要调用的内存分配器接口
*  @{
*/
#ifndef _DDK_INPUT_H_
#define _DDK_INPUT_H_

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

struct ddk_input_handle
{
	void *drv_handle;

	void (*event)(struct ddk_input_handle *handle, unsigned int event_type, unsigned int event_code, int value);
	void (*start)(struct ddk_input_handle *handle);
	void (*disconnect)(struct ddk_input_handle *handle);
};

#endif

/** @} */


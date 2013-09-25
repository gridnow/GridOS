/**
*  @defgroup net_rt
*  @ingroup DDK
*
*  定义了网络设备收发接口
*  @{
*/

#ifndef _DDK_NET_RT_H_
#define _DDK_NET_RT_H_

#include <list.h>
#include <compiler.h>
#include <ddk/net/ip/ip.h>

#include "define.h"

struct ddk_net_device;
#define DDK_NET_BUF_MAX_SLICE 16

/**
	@brief A buffer list to be sent or received
*/
struct ddk_net_buf
{
	struct list_head list;

	/* Descriptor */
	struct ddk_net_buf_desc
	{
		unsigned char nr_frags;
		unsigned short gso_size;
	}desc;

	/* 网络数据包头，目前我们就支持了IP 系列 */
	union 
	{
		struct iphdr network_hdr_ip;
	}network_hdr;

	size_t len, data_len/*非线性数据区的长度*/;
	void *data;
	__u8	ip_summed:2;

	void *buf[DDK_NET_BUF_MAX_SLICE];
	int last_buf;
	size_t last_size;
	struct ddk_net_device *dev;
};

typedef struct ddk_net_buf_frag skb_frag_t;

struct ddk_net_buf_frag {
	struct {
		struct page *p;
	} page;
#if (BITS_PER_LONG > 32) || (PAGE_SIZE >= 65536)
	__u32 page_offset;
	__u32 size;
#else
	__u16 page_offset;
	__u16 size;
#endif
};

/**
	@brief 发送数据

	@param[in] dev 选择发送的有效网络设备对象
	@param[in] buf 要发送的有效数据包链表
 
	@return
		netdev_tx_t 状态码
*/
DLLEXPORT netdev_tx_t netdev_start_transmit(struct ddk_net_device *dev, struct ddk_net_buf *buf);

/**
	@brief 分配网络缓冲区描述符
 
	@param[in] dev 有效网络设备对象
 
	@return
		网络缓冲区描述符，失败则是NULL
*/
DLLEXPORT struct ddk_net_buf *netdev_nb_alloc(struct ddk_net_device *dev);

/**
	@brief 释放网络缓冲区描述符
 
	在释放描述的过程中，如果该描述符携带数据，则这些数据也将被释放。
 
	@param[in] buf 有效网络缓冲区描述符
*/
DLLEXPORT void netdev_nb_dealloc(struct ddk_net_buf *buf);

/**
	@brief 向网络缓冲区描述符追加数据
 
	对于新的网络缓冲区描述符，其中是没有数据的，当要收发数据时，数据应该传入到该缓冲区中。
 
	@param[in] buf 有效网络缓冲区描述符
 	@param[in] data 有效数据指针
	@param[in] size 数据长度
 
	@return
		0 表示成功，< 0表示失败的错误码
 */
DLLEXPORT int netdev_nb_append_data(struct ddk_net_buf *buf, void *data, size_t size);

#endif
/** @} */

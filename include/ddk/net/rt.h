/**
*  @defgroup net_rt
*  @ingroup DDK
*
*  �����������豸�շ��ӿ�
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

	/* �������ݰ�ͷ��Ŀǰ���Ǿ�֧����IP ϵ�� */
	union 
	{
		struct iphdr network_hdr_ip;
	}network_hdr;

	size_t len, data_len/*�������������ĳ���*/;
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
	@brief ��������

	@param[in] dev ѡ���͵���Ч�����豸����
	@param[in] buf Ҫ���͵���Ч���ݰ�����
 
	@return
		netdev_tx_t ״̬��
*/
DLLEXPORT netdev_tx_t netdev_start_transmit(struct ddk_net_device *dev, struct ddk_net_buf *buf);

/**
	@brief �������绺����������
 
	@param[in] dev ��Ч�����豸����
 
	@return
		���绺������������ʧ������NULL
*/
DLLEXPORT struct ddk_net_buf *netdev_nb_alloc(struct ddk_net_device *dev);

/**
	@brief �ͷ����绺����������
 
	���ͷ������Ĺ����У������������Я�����ݣ�����Щ����Ҳ�����ͷš�
 
	@param[in] buf ��Ч���绺����������
*/
DLLEXPORT void netdev_nb_dealloc(struct ddk_net_buf *buf);

/**
	@brief �����绺����������׷������
 
	�����µ����绺������������������û�����ݵģ���Ҫ�շ�����ʱ������Ӧ�ô��뵽�û������С�
 
	@param[in] buf ��Ч���绺����������
 	@param[in] data ��Ч����ָ��
	@param[in] size ���ݳ���
 
	@return
		0 ��ʾ�ɹ���< 0��ʾʧ�ܵĴ�����
 */
DLLEXPORT int netdev_nb_append_data(struct ddk_net_buf *buf, void *data, size_t size);

#endif
/** @} */

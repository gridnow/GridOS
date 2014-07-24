
#include <ddk/input.h>
#include <ddk/slab.h>
#include <kernel/ke_atomic.h>
static LIST_HEAD(dev_list_hd);

/**
	@brief input the kbd/mouse/touch screen input stream

	@note
		����ͬһ�����󲻿�����		
*/
static int __ifi_input_stream(struct ifi_device * dev, void * buf, size_t size)
{	
	if (dev && dev->ifi_input_stream)
	{
		return dev->ifi_input_stream(dev, buf, size);
	}

	return 0;
}


/*
*	@brief input the kbd/mouse/touch screen input stream

	@note
		����ͬһ�����󲻿�����
*/

int ifi_input_stream(int type, void * buf, size_t size)
{
	struct ifi_device * dev = get_ifi_dev_by_devtype(type);
	return __ifi_input_stream(dev , buf, size);
}



int ifi_read_input(void * input, int type)
{
	struct ifi_device * dev = get_ifi_dev_by_devtype(type);
	
	/* No device ? */
	if (!dev)
		return 0;

	/*No method ?*/
	if (!dev->dev_ops || !dev->dev_ops->ifi_dev_read)
		return 0;
	return dev->dev_ops->ifi_dev_read(dev, (void *)input, 1);
}


/**
	@brief Create IFI device
*/
struct ifi_device * ifi_device_create(void)
{
	struct ifi_device * dev = (struct ifi_device *)hal_malloc(sizeof(*dev));
	if (!dev) return NULL;
	memset(dev, 0, sizeof(*dev));
	
	ke_init_completion(&dev->data_ready);
	ke_atomic_set(&dev->free_count, IFI_DEVICE_PKG_COUNT);
	//��ʼ���豸����,�����뵽�豸����ͷ
	INIT_LIST_HEAD(&(dev->dev_list));
	list_add(&(dev->dev_list), &dev_list_hd);
	
	return dev;
}

/**
**�����豸����������ȡ�豸ʵ��dev
**/
struct ifi_device * get_ifi_dev_by_devtype(int type)
{
	struct ifi_device *ifi_dev = NULL;
	//����dev_list_hd
	list_for_each_entry(ifi_dev, &dev_list_hd, dev_list)
	{
		if (ifi_dev->dev_type == type)
		{
			return ifi_dev;
		}
	}
	//û�и�����
	return NULL;
}


/**
	@brief Module init
*/
void init_input()
{
	
}

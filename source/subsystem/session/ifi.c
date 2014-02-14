
#include <ddk/input.h>

static LIST_HEAD(dev_list_hd);

/**
	@brief input the kbd/mouse/touch screen input stream

	@note
		对于同一个对象不可重入		
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
		对于同一个对象不可重入
*/

int ifi_input_stream(int type, void * buf, size_t size)
{
	struct ifi_device * dev = get_ifi_dev_by_devtype(type);
	return __ifi_input_stream(dev , buf, size);
}


#if 1
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
#endif

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
	//初始化设备链表,并加入到设备链表头
	INIT_LIST_HEAD(&(dev->dev_list));
	list_add(&(dev->dev_list), &dev_list_hd);
	
	return dev;
}

/**
**根据设备的类型来获取设备实例dev
**/
struct ifi_device * get_ifi_dev_by_devtype(int type)
{
	struct ifi_device *ifi_dev = NULL;
	//遍历dev_list_hd
	list_for_each_entry(ifi_dev, &dev_list_hd, dev_list)
	{
		if (ifi_dev->dev_type == type)
		{
			return ifi_dev;
		}
	}
	//没有该类型
	return NULL;
}


/**
	@brief Module init
*/
void init_input()
{
	
}

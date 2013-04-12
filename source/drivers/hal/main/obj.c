/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   Device Object Manager
*/

#include <types.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <list.h>

#include <kernel/ke_lock.h>
#include <ddk/compatible.h>

#include <ddk/slab.h>
#include <ddk/obj.h>
#include <ddk/log.h>

#include "object.h"

#define DEVICE_OBJECT_MAGGIC 0x55AA
struct device_object
{
	unsigned long magic;
	struct list_head list;
	struct device_object * parent;

	/* ����豸ƥ��Ĳ������������豸�������ṩ */
	struct do_device_type * type;
};

/**
	@brief �������������
*/
struct device_driver
{
	/* �ض������� */
	driver_t private;

	struct list_head list;
	struct do_device_type * type;										// ĳ�豸�������䱾����
};

static LIST_HEAD(devices_list);
static LIST_HEAD(drivers_list);
static DEFINE_SPINLOCK(devices_lock);
static DEFINE_SPINLOCK(drivers_lock);

static bool alloc_space(struct cl_object_type *type, void **base, size_t *size, enum cl_object_memory_type memory_type)
{
	void *object_memory;
	int count = 0;

	switch (memory_type)
	{
	case COMMON_OBJECT_MEMORY_TYPE_NAME:
	case COMMON_OBJECT_MEMORY_TYPE_OBJ:
	case COMMON_OBJECT_MEMORY_TYPE_NODE:
		count = 1;
		break;
	}
	if (count == 0) goto err;

	object_memory = hal_malloc(PAGE_SIZE * count);
	if (!object_memory) goto err;
	*base = object_memory;
	*size = count * PAGE_SIZE;
	
	return true;

err:
	return false;
};

static void free_space(struct cl_object_type *type, void *base, size_t size, enum cl_object_memory_type memory_type)
{

}

static bool object_close(real_object_t *obj)
{

}

static void object_init(real_object_t *obj)
{
	
}

static struct cl_object_ops device_object_ops = {
	.close				= object_close,
	.init				= object_init,
};

static struct cl_object_type device_type_template = {
	.name		= "�豸����",
	.size		= 0,
	.ops		= &device_object_ops,
	.add_space	= alloc_space,
	.free_space	= free_space,
};

static void lock_devices()
{
	ke_spin_lock(&devices_lock);
}

static void unlock_devices()
{
	ke_spin_unlock(&devices_lock);
}

static void lock_drivers()
{
	ke_spin_lock(&drivers_lock);
}

static void unlock_drivers()
{
	ke_spin_unlock(&drivers_lock);
}

static bool is_device_managed(struct device_object *dev)
{
	return dev->magic == DEVICE_OBJECT_MAGGIC;
}

static struct cl_object_type *get_raw_type(struct do_device_type *type)
{
	struct cl_object_type *p = (struct cl_object_type*)((void*)&type->dummy);

	/* RAW����Ҫ��ʼ����? ��һ��Ҫ��ʼ���û���TYPE */
	if (type->dummy == 0)
	{
		memcpy(p, &device_type_template, sizeof(*p));
		p->size = sizeof(struct device_object) + type->size_of_device;
		cl_object_type_register(p);
	}
	
	return p;
}

static struct device_object *device2manager(device_t device)
{
	struct device_object * dev = (struct device_object *)device;
	--dev;

	if (is_device_managed(dev) == false)
		return NULL;
	return dev;
}

static device_t manager2device(struct device_object *dev)
{
	return ++dev;
}

static struct device_driver *alloc_driver()
{
	return kzalloc(sizeof(struct device_driver), 0);
}

/**
	@brief Get the device by external pointer and hold a pointer to it
*/
static struct device_object * get_device(device_t who)
{
	struct device_object * dev;

	lock_devices();
	dev = device2manager(who);
	if (dev)
		cl_object_inc_ref(dev);
	unlock_devices();

	return dev;	
}

/**
	@brief Loop each device in the list

	@note
		The loop is locked
*/
static void loop_devices(bool (*fn)(struct device_object*, void *), void * ctx)
{
	struct device_object * dev;

	lock_devices();

	list_for_each_entry(dev, &devices_list, list)
	{
		bool r;

		/* Add the reference counter to prevent its deletion */
		cl_object_inc_ref(dev);

		/* Unlock the device list, so the callback can use the interface of manager */
		unlock_devices();
		r = fn(dev, ctx);
		lock_devices();

		/* Dec the reference counter to restore the original status */
		cl_object_dec_ref(dev);
		
		if (r == false)
			break;
	}

	unlock_devices();
}

/**
	@brief Find a driver by external pointer in non-lock mode

	@note
		If you want the result to be consistent ,locking the manager is needed.
*/
static struct device_driver * find_driver(void * who)
{
	struct device_driver * drv;

	list_for_each_entry(drv, &drivers_list, list)
	{
		if (drv->private == who)
			return drv;
	}

	return NULL;
}

static bool match_driver(struct device_object * device, void * ctx)
{
	struct device_driver * drv = ctx;

	/* û��ƥ�䷽����*/
	if (!device->type->match)
		goto nomatch;

	/* ���ͶԲ��ԣ�*/
	if (device->type != drv->type)
		goto nomatch;

	if (device->type->match(manager2device(device), drv->private) == true)
		goto matched;

nomatch:
	return true;

	/* ���ƥ��ɹ�����ֹͣѭ�� */
matched:
	return false;
}

DLLEXPORT bool do_register_driver(void * driver, struct do_device_type * type)
{
	struct device_driver * drv = NULL, *p;

	/* Get a new device */
	drv = alloc_driver();
	if (!drv)
		goto err1;

	/* Init the device object */
	drv->private = driver;
	drv->type = type;

	/* Insert to the list */
	lock_drivers();
	list_add_tail(&drv->list, &drivers_list);
	unlock_drivers();

	/* Matching... */
	loop_devices(match_driver, drv);

	return true;

err2:
err1:
	return false;
}

DLLEXPORT bool do_unregister_driver(void * driver, struct do_device_type * type)
{
	
}

DLLEXPORT bool do_register_device(device_t device, device_t parent)
{
	struct device_object * dev = NULL, *p;

	/* Get a new device */
	dev = (struct device_object*)device - 1;
	if (dev->magic != DEVICE_OBJECT_MAGGIC)
		goto err1;

	/* If we have the parent, find it and link to it */
	if (parent)
	{
		p = get_device(parent);
		if (!p)
			goto err2;
		dev->parent = p;
	}

	/* Insert to the list */
	lock_devices();
	list_add_tail(&dev->list, &devices_list);
	unlock_devices();

	//printk("devmgr: register device %s.\n", )
	return true;

err2:	
err1:
	return false;
}

DLLEXPORT void do_unregister_device(void * device)
{
	TODO("");
}

/**
	@brief �����ض����͵��豸
*/
DLLEXPORT device_t do_find_device(struct do_device_type * type, device_t start, void * data, 
								  bool (*match)(device_t dev, void *data))
{
	struct device_object * dev;
	device_t device = NULL;

	int really_match = 0;

	/* �տ�ʼ��������Խ���ƥ�䣬������������start�豸�ſ�ʼƥ�� */
	if (start == NULL)
		really_match = 1;

	/* ����������� */
	lock_devices();

	list_for_each_entry(dev, &devices_list, list)
	{
		if (dev->type == type)
		{
			/* �Ѿ�����start�㣬�����û���ƥ�����̣�ƥ��ɹ����������� */
			if (really_match)
			{
				if (match(manager2device(dev), data))
				{
					device = manager2device(dev);
					cl_object_inc_ref(dev);
					break;
				}
			}
			else
			{
				if (manager2device(dev) == start)
					really_match = 1;
			}
		}
	}

	unlock_devices();
	return device;	
}

DLLEXPORT bool do_set_device_name(device_t device, const char *fmt, ...)
{	
	va_list args;
	struct device_object *dev = device2manager(device);
	if (!dev) goto err1;

	va_start(args, fmt);
	//TODO: vsnprintf(dst_name, NAME_LENGTH, fmt, args);
	va_end(args);

	return true;
	
err1:
	printk("%s: û���ҵ��豸%x.\n", __FUNCTION__, device);
	return false;
}

DLLEXPORT char *do_get_device_name(device_t device)
{
	struct device_object *dev = device2manager(device);
	if (!dev) goto err1;
	
	//TODO: TO support device name

err1:
	return "��δ֪�豸����";
}

/**
	@brief �����豸
*/
DLLEXPORT device_t do_get_device(device_t device)
{
	struct device_object *dev;
		
	dev = get_device(device);
	if (!dev) return NULL;
	return device;
}

/**
	@brief ȡ�����豸������
*/
DLLEXPORT device_t do_put_device(device_t device)
{
	struct device_object *dev = device2manager(device);
	if (!dev) goto err;
	
	cl_object_dec_ref(dev);
	return device;
	
err:
	return NULL;
}

DLLEXPORT void *do_alloc_raw(struct do_device_type *type)
{
	struct cl_object_type *raw_type = get_raw_type(type);
	struct device_object *dev = cl_object_create(raw_type);
	if (!dev) return NULL;

	dev->magic = DEVICE_OBJECT_MAGGIC;
	dev->type = type;
	
	return manager2device(dev);
}

DLLEXPORT do_close_device(device_t device)
{
}

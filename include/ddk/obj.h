/**
*  @defgroup Object
*  @ingroup DDK
*
*  定义了驱动、设备对象管理器的接口
*  @{
*/

#ifndef _DDK_OBJ_H_
#define _DDK_OBJ_H_

#include <ddk/types.h>

typedef void * device_t;
typedef void * driver_t;

/**
	@brief 与设备类型相关的操作方法，由设备发现者提供 
*/
struct do_device_type
{
	const char * name;
	int size_of_device;
	
	/**
		@brief 该类型设备与驱动的匹配方法
	*/
	bool (*match)(device_t device, driver_t driver);				/**< 驱动匹配接口，设备管理器对于每个新注册的设备进行驱动的匹配，这是具体的匹配方法 */

	bool (*delete)(device_t device);								/**< 该类设备的删除方法，驱动管理器会在设备引用计数器为0的情况下调用该接口。
																		如果设备被销毁而驱动程序还在用，那么驱动系统会混乱，但是这一般是由于驱动编写不健壮而引起的
																	*/
	/**
		@brief 探测某个设备是否是该类型的
	*/
	bool (*probe)(device_t device);

	/* Not filled by driver, reserved for manager (cl_object_type) */
	unsigned long dummy;
	unsigned char dummy_byte[256];	
};

/**
	@brief 注册设备对象到设备管理器中

	在设备管理器中添加一个新设备，并匹配存在的驱动程序，如果匹配成功则启动驱动程序使用该硬件。

	@param[in] device 要注册的新设备
	@param[in] parent 新设备的父设备	

	@return
		成功返回true，失败返回false，如果该设备曾经注册过，那么返回false

	@note
		1,如果parent存在，那么parent将被增加一次引用，因此，要删除parent先得删除device
		2,type必须由驱动发现者提供，描述该设备的类型信息和类型操作方法
*/
bool do_register_device(device_t device, device_t parent);

/**
	@brief 从设备管理器中撤销一个设备的注册信息
*/
void do_unregister_device(void * device);

/**
	@brief 注册驱动到设备管理器中

	在设备管理器中添加一个新的驱动程序描述信息，并调用该驱动的匹配方法匹配设备管理器中注册那些没有对接驱动的设备。
	如果驱动的匹配方法能成功识别硬件，则该驱动与该设备建立起“驱动”关系。

	@param[in] driver 驱动程序描述体，如ddk_pci_driver
	@param[in] type 该驱动程序能驱动起的设备类型，如果一般通过调用特定总线的接口获取

	@return
		true 注册成功，false注册失败
*/
bool do_register_driver(void * driver, struct do_device_type * type);

/**
	@brief 设置设备对象的名字
*/
bool do_set_device_name(device_t device, const char *fmt, ...);

/**
	@brief 获取设备对象的名字
*/
char *do_get_device_name(device_t device);

/**
	@brief 释放设备的引用指针
*/
device_t do_put_device(device_t device);

/**
	@brief 分配设备对象的地址
*/
void *do_alloc_raw(struct do_device_type *type);

/**
	@brief 驱动中要增加一个引用指针
*/
device_t do_get_device(device_t device);

/**
	@brief 遍历特定类型的设备

	@param[in] type 要搜索的设备类型
	@param[in] start 要开始搜索的设备，如果为NULL，则从该类型的第一个设备开始遍历
	@param[in] data 给匹配函数的参数，驱动自定义
	@param[in] match 匹配回掉函数，如果返回true,则停止遍历

	@return	
		返回匹配成功的设备,NULL表示终止或失败
*/
device_t do_find_device(struct do_device_type * type, device_t start, void * data, bool (*match)(device_t dev, void *data));


//@param[in] type 新设备的类型描述信息和该类型设备的操作方法
#endif

/** @} */

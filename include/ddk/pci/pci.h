/**
*  @defgroup PCIInterface
*  @ingroup DDK
*
*  定义了PCI总线驱动提供的编程接口
*  @{
*/

#ifndef _PCI_H_
#define _PCI_H_
#include <list.h>

#include <kernel/ke_lock.h>
#include <kernel/ke_atomic.h>

#include <ddk/types.h>
#include <ddk/compatible.h>
#include <ddk/compiler.h>

#define PCI_ANY_ID (~0)

struct pci_device_id {
	unsigned int vendor, device;													/**< Vendor and device ID or PCI_ANY_ID */
	unsigned int subvendor, subdevice;												/**< Subsystem ID's or PCI_ANY_ID */
	unsigned int class, class_mask;													/**< (class,subclass,prog-if) triplet */
	unsigned long driver_data;														/**< Data private to the driver */
};

#ifndef PCI_H_INTERNAL

/**
	@brief PCI 总线的定义，驱动程序可见
*/
struct pci_bus 
{
	unsigned char	number;		/* bus number */
	unsigned char	primary;	/* number of primary bridge */
	unsigned char	max_bus_speed;	/* enum pci_bus_speed */
	unsigned char	cur_bus_speed;	/* enum pci_bus_speed */
};

/**
	@brief PCI 设备的定义，驱动程序可见
*/
struct pci_dev
{
	unsigned int	devfn;															/**< encoded device & function index */
	unsigned short	vendor;
	unsigned short	device;
	unsigned short	subsystem_vendor;
	unsigned short	subsystem_device;
	unsigned int	class;															/**< 3 bytes: (base,sub,prog-if) */
	u8		revision;																/**< PCI revision, low byte of class word */
	u8		hdr_type;																/**< PCI header type (`multi' flag masked out) */
	u8		pcie_cap;	
	u8		pcie_type:4;
	u8		pcie_mpss:3;
	u8		rom_base_reg;
	u8		pin;  																	/**< which interrupt pin this device uses */

	struct pci_bus	*bus;															/**< bus this device is on */
	struct device dev;
	u64		dma_mask;
	unsigned int	irq;
};
#else
struct pci_dev;
#endif
#define to_pci_dev(n)  container_of(n, struct pci_dev, dev)

/**
	@brief PCI 驱动程序描述体
*/
struct ddk_pci_driver 
{
	const char *name;														/**< 驱动的常量名 */
	const struct pci_device_id *id_table;									/**< 不可为NULL，本驱动程序所支持的PCI设备列表 */
	int  (*probe)  (struct pci_dev *dev, const struct pci_device_id *id);	/**< 不可为NULL，PCI驱动必须提供该驱动所支持的设备匹配方法 */
	void (*remove) (struct pci_dev *dev);									/**< 设备准备被移除 */
};

/**
	@brief PCI驱动程序提供的设备修正方法，一般在PCI设备的各个环节中调用
*/
struct pci_fixup {
	struct list_head list;
	u16 vendor;																/**< 对哪家厂商的设备进行修正，PCI_ANY_ID则表示任何厂商 */
	u16 device;																/**< 对哪款设备进行修正，PCI_ANY_ID表示任何设备 */
	u32 class;																/**< 对哪类设备进行修正，PCI_ANY_ID表示任何类型 */
	unsigned int class_shift;												/**< Should be 0, 8, 16 */
	void (*hook)(struct pci_dev *dev);										/**< 修正函数 */
};

/**
	@brief 用于生成PCI设备厂商+设备ID的描述信息pci_device_id
*/
#define PCI_DEVICE(vend,dev) \
	.vendor = (vend), .device = (dev), \
	.subvendor = PCI_ANY_ID, .subdevice = PCI_ANY_ID

#define PCI_VDEVICE(vendor, device)		\
	PCI_VENDOR_ID_##vendor, (device),	\
	PCI_ANY_ID, PCI_ANY_ID, 0, 0

/**
	@brief 用于定义PCI驱动所支持的设备编号列表
*/
#define DEFINE_PCI_DEVICE_TABLE(_table) \
	const struct pci_device_id _table[] __devinitconst


/**
	@brief PCI资源编号
*/
enum {
	/* #0-5: standard PCI resources */
	PCI_STD_RESOURCES,
	PCI_STD_RESOURCE_END = 5,

	/* #6: expansion ROM resource */
	PCI_ROM_RESOURCE,
};

/*
 *	7:3 = slot
 *	2:0 = function
 */
#define PCI_DEVFN(slot, func)	((((slot) & 0x1f) << 3) | ((func) & 0x07))
#define PCI_SLOT(devfn)		(((devfn) >> 3) & 0x1f)
#define PCI_FUNC(devfn)		((devfn) & 0x07)

/*
 * Error values that may be returned by PCI functions.
 */
#define PCIBIOS_SUCCESSFUL		0x00
#define PCIBIOS_FUNC_NOT_SUPPORTED	0x81
#define PCIBIOS_BAD_VENDOR_ID		0x83
#define PCIBIOS_DEVICE_NOT_FOUND	0x86
#define PCIBIOS_BAD_REGISTER_NUMBER	0x87
#define PCIBIOS_SET_FAILED		0x88
#define PCIBIOS_BUFFER_TOO_SMALL	0x89

/*
	PCI 访问接口，返回值是上面定义的PCIBIOS_XXX
*/
int pci_read_config_byte(const struct pci_dev *dev, int where, u8 *val);
int pci_read_config_word(const struct pci_dev *dev, int where, u16 *val);
int pci_read_config_dword(const struct pci_dev *dev, int where, u32 *val);
int pci_write_config_byte(const struct pci_dev *dev, int where, u8 val);
int pci_write_config_word(const struct pci_dev *dev, int where, u16 val);
int pci_write_config_dword(const struct pci_dev *dev, int where, u32 val);

/**
	@brief 获取PCI设备的类型信息

	一般被驱动程序用来标识该驱动是个PCI设备驱动

	@return
		PCI设备类型描述符
*/
struct do_device_type * pci_get_device_type();

/**
	@brief 注册一个PCI设备驱动

	PCI驱动程序被加载到内存中开始运行，一般调用本接口将驱动注册到设备管理器中。
	一旦注册成功，驱动程序的PROBE接口就被设备管理器调用，用于匹配可能能匹配成功过的PCI设备。

	@param[in] drv PCI设备驱动描述信息

	@return
		成功返回true，失败返回false
*/
bool pci_register_driver(struct ddk_pci_driver * drv);

/**
	@brief 取消一个PCI驱动程序在设备管理器中的记录

	PCI驱动程序被将被卸载，首先要从设备管理器中销毁其记录信息。

	@param[in] drv PCI设备驱动描述信息
*/
void pci_unregister_driver(struct ddk_pci_driver *drv);

/**
	@brief 开启PCI设备
*/
int pci_enable_device(struct pci_dev *dev);

/**
	@brief 停止PCI设备
*/
void pci_disable_device(struct pci_dev *dev);

/**
	@brief 释放设备引用
*/
void pci_dev_put(struct pci_dev *dev);

/**
	@brief 设置PCI设备的DMA地址长度
*/
int pci_set_dma_mask(struct pci_dev *dev, u64 mask);

/**
	@brief 设置PCI设备的consistent DMA地址长度
*/
int pci_set_consistent_dma_mask(struct pci_dev *dev, u64 mask);

/**
	@brief 使能PCI设备
*/
int pcim_enable_device(struct pci_dev *pdev);

/**
	@brief <还未实现>有用吗？
*/
void pcim_pin_device(struct pci_dev *pdev);

/**
	@brief msi功能的使能
*/
int pci_enable_msi_block(struct pci_dev *dev, unsigned int nvec);
#define pci_enable_msi(pdev)	pci_enable_msi_block(pdev, 1)

/**
	@brief 还未实现
*/
void __iomem * const *pcim_iomap_table(struct pci_dev *pdev);

/**
	@brief enables/disables PCI INTx for device dev
*/
void pci_intx(struct pci_dev *dev, int enable);

/**
	@brief 设置PCI成为BUS MUSTER 允许的设备
*/
void pci_set_master(struct pci_dev *dev);

/**
	@brief 搜索设备

	根据编号搜索设备，如果搜索到则增加其引用计数

	@param[in] vendor PCI厂商ID
	@param[in] device PCI设备ID
	@param[in] from 从那个设备开始搜索，NULL则从全局PCI列表中搜索

	@return
		成功则返回匹配的设备，否则返回NULL
*/
struct pci_dev * pci_get_device(unsigned int vendor, unsigned int device, struct pci_dev *from);

/**
	@brief 映射PCI地址
*/
void __iomem *pci_ioremap_bar(struct pci_dev *pdev, int bar);

/**
	@brief 注册final设备修正
*/
void pci_register_final_quirk(struct pci_fixup * q);

/**
	@brief 获取PCI设备的资源FLAG
*/
struct resource * pci_resource(struct pci_dev * pdev);
#define pci_resource_flags(dev, bar) ((pci_resource(dev) + bar)->flags)
#define pci_resource_start(dev, bar) ((pci_resource(dev) + bar)->start)
#define pci_resource_end(dev, bar)	((pci_resource(dev) + bar)->end)
#define pci_resource_len(dev,bar) \
	((pci_resource_start((dev), (bar)) == 0 &&	\
	pci_resource_end((dev), (bar)) ==		\
	pci_resource_start((dev), (bar))) ? 0 :	\
	\
	(pci_resource_end((dev), (bar)) -		\
	pci_resource_start((dev), (bar)) + 1))

#endif

/** @} */

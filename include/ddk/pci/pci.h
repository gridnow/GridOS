/**
*  @defgroup PCIInterface
*  @ingroup DDK
*
*  ������PCI���������ṩ�ı�̽ӿ�
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
	@brief PCI ���ߵĶ��壬��������ɼ�
*/
struct pci_bus 
{
	unsigned char	number;		/* bus number */
	unsigned char	primary;	/* number of primary bridge */
	unsigned char	max_bus_speed;	/* enum pci_bus_speed */
	unsigned char	cur_bus_speed;	/* enum pci_bus_speed */
};

/**
	@brief PCI �豸�Ķ��壬��������ɼ�
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
	@brief PCI ��������������
*/
struct ddk_pci_driver 
{
	const char *name;														/**< �����ĳ����� */
	const struct pci_device_id *id_table;									/**< ����ΪNULL��������������֧�ֵ�PCI�豸�б� */
	int  (*probe)  (struct pci_dev *dev, const struct pci_device_id *id);	/**< ����ΪNULL��PCI���������ṩ��������֧�ֵ��豸ƥ�䷽�� */
	void (*remove) (struct pci_dev *dev);									/**< �豸׼�����Ƴ� */
};

/**
	@brief PCI���������ṩ���豸����������һ����PCI�豸�ĸ��������е���
*/
struct pci_fixup {
	struct list_head list;
	u16 vendor;																/**< ���ļҳ��̵��豸����������PCI_ANY_ID���ʾ�κγ��� */
	u16 device;																/**< ���Ŀ��豸����������PCI_ANY_ID��ʾ�κ��豸 */
	u32 class;																/**< �������豸����������PCI_ANY_ID��ʾ�κ����� */
	unsigned int class_shift;												/**< Should be 0, 8, 16 */
	void (*hook)(struct pci_dev *dev);										/**< �������� */
};

/**
	@brief ��������PCI�豸����+�豸ID��������Ϣpci_device_id
*/
#define PCI_DEVICE(vend,dev) \
	.vendor = (vend), .device = (dev), \
	.subvendor = PCI_ANY_ID, .subdevice = PCI_ANY_ID

#define PCI_VDEVICE(vendor, device)		\
	PCI_VENDOR_ID_##vendor, (device),	\
	PCI_ANY_ID, PCI_ANY_ID, 0, 0

/**
	@brief ���ڶ���PCI������֧�ֵ��豸����б�
*/
#define DEFINE_PCI_DEVICE_TABLE(_table) \
	const struct pci_device_id _table[] __devinitconst


/**
	@brief PCI��Դ���
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
	PCI ���ʽӿڣ�����ֵ�����涨���PCIBIOS_XXX
*/
int pci_read_config_byte(const struct pci_dev *dev, int where, u8 *val);
int pci_read_config_word(const struct pci_dev *dev, int where, u16 *val);
int pci_read_config_dword(const struct pci_dev *dev, int where, u32 *val);
int pci_write_config_byte(const struct pci_dev *dev, int where, u8 val);
int pci_write_config_word(const struct pci_dev *dev, int where, u16 val);
int pci_write_config_dword(const struct pci_dev *dev, int where, u32 val);

/**
	@brief ��ȡPCI�豸��������Ϣ

	һ�㱻��������������ʶ�������Ǹ�PCI�豸����

	@return
		PCI�豸����������
*/
struct do_device_type * pci_get_device_type();

/**
	@brief ע��һ��PCI�豸����

	PCI�������򱻼��ص��ڴ��п�ʼ���У�һ����ñ��ӿڽ�����ע�ᵽ�豸�������С�
	һ��ע��ɹ������������PROBE�ӿھͱ��豸���������ã�����ƥ�������ƥ��ɹ�����PCI�豸��

	@param[in] drv PCI�豸����������Ϣ

	@return
		�ɹ�����true��ʧ�ܷ���false
*/
bool pci_register_driver(struct ddk_pci_driver * drv);

/**
	@brief ȡ��һ��PCI�����������豸�������еļ�¼

	PCI�������򱻽���ж�أ�����Ҫ���豸���������������¼��Ϣ��

	@param[in] drv PCI�豸����������Ϣ
*/
void pci_unregister_driver(struct ddk_pci_driver *drv);

/**
	@brief ����PCI�豸
*/
DLLEXPORT int pci_enable_device(struct pci_dev *dev);

/**
	@brief ֹͣPCI�豸
*/
void pci_disable_device(struct pci_dev *dev);

/**
	@brief �ͷ��豸����
*/
void pci_dev_put(struct pci_dev *dev);

/**
	@brief ����PCI�豸��DMA��ַ����
*/
DLLEXPORT int pci_set_dma_mask(struct pci_dev *dev, u64 mask);

/**
	@brief ����PCI�豸��consistent DMA��ַ����
*/
DLLEXPORT int pci_set_consistent_dma_mask(struct pci_dev *dev, u64 mask);

/**
	@brief ʹ��PCI�豸
*/
DLLEXPORT int pcim_enable_device(struct pci_dev *pdev);

/**
	@brief <��δʵ��>������
*/
void pcim_pin_device(struct pci_dev *pdev);

/**
	@brief ��δʵ��
*/
void __iomem * const *pcim_iomap_table(struct pci_dev *pdev);

/**
	@brief enables/disables PCI INTx for device dev
*/
void pci_intx(struct pci_dev *dev, int enable);

/**
	@brief ����PCI��ΪBUS MUSTER ������豸
*/
void pci_set_master(struct pci_dev *dev);

/**
	@brief �����豸

	���ݱ�������豸����������������������ü���

	@param[in] vendor PCI����ID
	@param[in] device PCI�豸ID
	@param[in] from ���Ǹ��豸��ʼ������NULL���ȫ��PCI�б�������

	@return
		�ɹ��򷵻�ƥ����豸�����򷵻�NULL
*/
struct pci_dev * pci_get_device(unsigned int vendor, unsigned int device, struct pci_dev *from);

/**
	@brief �����豸��cache����
 
	@param[in] dev ����Ҫ�����ĸ�pci�豸������
 
	@return �ɹ�����0�����򷵻ش�����
*/
DLLEXPORT int pci_set_mwi(struct pci_dev *dev);

/**
	@brief Ϊһ��pci�豸�������е��豸��Դ
 
	һ����˵��pci�豸�����������豸��Դ���ڶ���ӳ�����
 
	@brief dev Ҫ������pci�豸����
	@brief res_name �������ߵ�����
 
	@return �ɹ�����0�����򷵻ش�����
*/
DLLEXPORT int pci_request_regions(struct pci_dev *dev, const char *res_name);


/**
	@brief ӳ��PCI��ַ
*/
void __iomem *pci_ioremap_bar(struct pci_dev *pdev, int bar);

/**
	@brief ע��final�豸����
*/
void pci_register_final_quirk(struct pci_fixup * q);

/**
	@brief ��ȡPCI�豸����ԴFLAG
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

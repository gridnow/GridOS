#ifndef PCI_H_INTERNAL
#define PCI_H_INTERNAL

#include <list.h>
#include <compiler.h>
#include <errno.h>

#include <ddk/types.h>
#include <ddk/resource.h>
#include <ddk/pci/pci.h>
#include <ddk/obj.h>
#include <ddk/compatible_atomic.h>

extern u8 pci_dfl_cache_line_size;
extern u8 pci_cache_line_size;

#define PCI_CFG_SPACE_SIZE	256
#define PCI_CFG_SPACE_EXP_SIZE	4096

/*
 *  For PCI devices, the region numbers are assigned this way:
 */
enum {
	/* #0-5: standard PCI resources */
	/* Defined in Export file */
	_PCI_STD_RESOURCES,
	_PCI_STD_RESOURCE_END = 5,

	/* #6: expansion ROM resource */
	_PCI_ROM_RESOURCE,

	/* device specific resources */
#ifdef CONFIG_PCI_IOV
	PCI_IOV_RESOURCES,
	PCI_IOV_RESOURCE_END = PCI_IOV_RESOURCES + PCI_SRIOV_NUM_BARS - 1,
#endif

	/* resources assigned to buses behind the bridge */
#define PCI_BRIDGE_RESOURCE_NUM 4

	PCI_BRIDGE_RESOURCES,
	PCI_BRIDGE_RESOURCE_END = PCI_BRIDGE_RESOURCES +
				  PCI_BRIDGE_RESOURCE_NUM - 1,

	/* total resources associated with a PCI device */
	PCI_NUM_RESOURCES,

	/* preserve this for compatibility */
	DEVICE_COUNT_RESOURCE = PCI_NUM_RESOURCES,
};

typedef int __bitwise pci_power_t;

#define PCI_D0		((pci_power_t __force) 0)
#define PCI_D1		((pci_power_t __force) 1)
#define PCI_D2		((pci_power_t __force) 2)
#define PCI_D3hot	((pci_power_t __force) 3)
#define PCI_D3cold	((pci_power_t __force) 4)
#define PCI_UNKNOWN	((pci_power_t __force) 5)
#define PCI_POWER_ERROR	((pci_power_t __force) -1)

typedef unsigned short __bitwise pci_bus_flags_t;
enum pci_bus_flags {
	PCI_BUS_FLAGS_NO_MSI   = (__force pci_bus_flags_t) 1,
	PCI_BUS_FLAGS_NO_MMRBC = (__force pci_bus_flags_t) 2,
};

#define PCI_PM_D2_DELAY		200
#define PCI_PM_D3_WAIT		10
#define PCI_PM_D3COLD_WAIT	100
#define PCI_PM_BUS_WAIT		50

/** The pci_channel state describes connectivity between the CPU and
 *  the pci device.  If some PCI bus between here and the pci device
 *  has crashed or locked up, this info is reflected here.
 */
typedef unsigned int __bitwise pci_channel_state_t;

enum pci_channel_state {
	/* I/O channel is in normal state */
	pci_channel_io_normal = (__force pci_channel_state_t) 1,

	/* I/O to channel is blocked */
	pci_channel_io_frozen = (__force pci_channel_state_t) 2,

	/* PCI card is dead */
	pci_channel_io_perm_failure = (__force pci_channel_state_t) 3,
};

typedef unsigned short __bitwise pci_dev_flags_t;
enum pci_dev_flags {
	/* INTX_DISABLE in PCI_COMMAND register disables MSI
	 * generation too.
	 */
	PCI_DEV_FLAGS_MSI_INTX_DISABLE_BUG = (__force pci_dev_flags_t) 1,
	/* Device configuration is irrevocably lost if disabled into D3 */
	PCI_DEV_FLAGS_NO_D3 = (__force pci_dev_flags_t) 2,
	/* Provide indication device is assigned by a Virtual Machine Manager */
	PCI_DEV_FLAGS_ASSIGNED = (__force pci_dev_flags_t) 4,
};

/* Based on the PCI Hotplug Spec, but some values are made up by us */
enum pci_bus_speed {
	PCI_SPEED_33MHz			= 0x00,
	PCI_SPEED_66MHz			= 0x01,
	PCI_SPEED_66MHz_PCIX		= 0x02,
	PCI_SPEED_100MHz_PCIX		= 0x03,
	PCI_SPEED_133MHz_PCIX		= 0x04,
	PCI_SPEED_66MHz_PCIX_ECC	= 0x05,
	PCI_SPEED_100MHz_PCIX_ECC	= 0x06,
	PCI_SPEED_133MHz_PCIX_ECC	= 0x07,
	PCI_SPEED_66MHz_PCIX_266	= 0x09,
	PCI_SPEED_100MHz_PCIX_266	= 0x0a,
	PCI_SPEED_133MHz_PCIX_266	= 0x0b,
	AGP_UNKNOWN			= 0x0c,
	AGP_1X				= 0x0d,
	AGP_2X				= 0x0e,
	AGP_4X				= 0x0f,
	AGP_8X				= 0x10,
	PCI_SPEED_66MHz_PCIX_533	= 0x11,
	PCI_SPEED_100MHz_PCIX_533	= 0x12,
	PCI_SPEED_133MHz_PCIX_533	= 0x13,
	PCIE_SPEED_2_5GT		= 0x14,
	PCIE_SPEED_5_0GT		= 0x15,
	PCIE_SPEED_8_0GT		= 0x16,
	PCI_SPEED_UNKNOWN		= 0xff,
};

/* pci_slot represents a physical slot */
struct pci_slot {
	struct pci_bus *bus;		/* The bus this slot is on */
	struct list_head list;		/* node in list of slots on this bus */
//	struct hotplug_slot *hotplug;	/* Hotplug info (migrate over time) */
	unsigned char number;		/* PCI_SLOT(pci_dev->devfn) */
//	struct kobject kobj;
};

struct pci_dev
{
	/* 与导出的部分（DDK部分）必须一致，DDK为驱动程序访问PCI_DEV提供接口 */
	unsigned int	devfn;		/* encoded device & function index */
	unsigned short	vendor;
	unsigned short	device;
	unsigned short	subsystem_vendor;
	unsigned short	subsystem_device;
	unsigned int	class;		/* 3 bytes: (base,sub,prog-if) */
	u8		revision;	/* PCI revision, low byte of class word */
	u8		hdr_type;	/* PCI header type (`multi' flag masked out) */
	u8		pcie_cap;	/* PCI-E capability offset */
	u8		pcie_type:4;	/* PCI-E device/port type */
	u8		pcie_mpss:3;	/* PCI-E Max Payload Size Supported */
	u8		rom_base_reg;	/* which config register controls the ROM */
	u8		pin;  		/* which interrupt pin this device uses */

	struct pci_bus	*bus;		/* bus this device is on */
	struct device dev;			/* 还是潜入了一个假的的struct device 对象，由于一些上层的驱动用dev成员来转换到pci_dev，所以需要该域作为地址索引。*/
	u64		dma_mask;	/* Mask of the bits of bus address this
					   device implements.  Normally this is
					   0xffffffff.  You only need to change
					   this if your device has broken DMA
					   or supports 64-bit transfers.  */
	unsigned int	irq;

	/* 驱动不可见部分 */
	struct list_head bus_list;	/* node in per-bus list */
	struct pci_bus	*subordinate;	/* bus this device bridges to(如果该设备是桥，那么该变量表示该桥所挂接的总线设备) */

	struct pci_slot	*slot;		/* Physical slot this device is in */

	
	pci_power_t     current_state;  /* Current operating state. In ACPI-speak,
					   this is D0-D3, D0 being fully functional,
					   and D3 being off. */

	int		pm_cap;		/* PM capability offset in the
					   configuration space */
	unsigned int	pme_support:5;	/* Bitmask of states from which PME#
					   can be generated */
	unsigned int	pme_interrupt:1;
	unsigned int	pme_poll:1;	/* Poll device's PME status bit */
	unsigned int	d1_support:1;	/* Low power state D1 is supported */
	unsigned int	d2_support:1;	/* Low power state D2 is supported */
	unsigned int	no_d1d2:1;	/* D1 and D2 are forbidden */
	unsigned int	no_d3cold:1;	/* D3cold is forbidden */
	unsigned int	d3cold_allowed:1;	/* D3cold is allowed by user */
	unsigned int	mmio_always_on:1;	/* disallow turning off io/mem
						   decoding during bar sizing */
	unsigned int	wakeup_prepared:1;
	unsigned int	runtime_d3cold:1;	/* whether go through runtime
						   D3cold, not set for devices
						   powered on/off by the
						   corresponding bridge */
	unsigned int	d3_delay;	/* D3->D0 transition time in ms */
	unsigned int	d3cold_delay;	/* D3cold->D0 transition time in ms */

	pci_channel_state_t error_state;	/* current connectivity state */

	int		cfg_size;	/* Size of configuration space */

	struct resource resource[DEVICE_COUNT_RESOURCE]; /* I/O and memory regions + expansion ROMs */

	/* These fields are used by common fixups */
	unsigned int	transparent:1;	/* Transparent PCI bridge */
	unsigned int	multifunction:1;/* Part of multi-function device */
	unsigned int	is_added:1;
	unsigned int	is_busmaster:1; /* device is busmaster */

	unsigned int	ari_enabled:1;	/* ARI forwarding */
	unsigned int    is_hotplug_bridge:1;
	unsigned int	io_window_1k:1;	/* Intel P2P bridge 1K I/O windows */
	pci_dev_flags_t dev_flags;

	atomic_t	enable_cnt;	/* pci_enable_device has been called */

};

enum pci_bar_type {
	pci_bar_unknown,	/* Standard PCI BAR probe */
	pci_bar_io,		/* An io port BAR */
	pci_bar_mem32,		/* A 32-bit memory BAR */
	pci_bar_mem64,		/* A 64-bit memory BAR */
};

/* Low-level architecture-dependent routines */

struct pci_ops 
{
	int (*read)(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 *val);
	int (*write)(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 val);
};

#define PCI_REGION_FLAG_MASK	0x0fU	/* These bits of resource flags tell us the PCI region flags */

struct pci_bus 
{
	/* 驱动可见部分 */
	unsigned char	number;		/* bus number */
	unsigned char	primary;	/* number of primary bridge */
	unsigned char	max_bus_speed;	/* enum pci_bus_speed */
	unsigned char	cur_bus_speed;	/* enum pci_bus_speed */

	/* 驱动不可见部分 */
	struct list_head node;		/* node in list of buses */
	struct pci_bus	*parent;	/* parent bus this bridge is on */
	struct list_head children;	/* list of child buses */
	struct list_head devices;	/* list of devices on this bus */
	struct pci_dev	*self;		/* bridge device as seen by parent */
	struct list_head slots;		/* list of slots on this bus */
	struct resource *resource[PCI_BRIDGE_RESOURCE_NUM];
	struct list_head resources;	/* address space routed to this bus */
	struct resource busn_res;	/* bus numbers routed to this bus */

	struct pci_ops	*ops;		/* configuration access functions */
	void		*sysdata;	/* hook for sys-specific extension */

	char		name[48];

	unsigned short  bridge_ctl;	/* manage NO_ISA/FBB/et al behaviors */
	pci_bus_flags_t bus_flags;	/* Inherited by child busses */
	void		*bridge;		/* 该总线被挂接到的桥pci_host_bridge */
	unsigned int		is_added:1;
};

struct pci_bus_region {
	resource_size_t start;
	resource_size_t end;
};

enum pci_dma_burst_strategy {
	PCI_DMA_BURST_INFINITY,	/* make bursts as large as possible,
				   strategy_parameter is N/A */
	PCI_DMA_BURST_BOUNDARY, /* disconnect at every strategy_parameter
				   byte boundaries */
	PCI_DMA_BURST_MULTIPLE, /* disconnect at some multiple of
				   strategy_parameter byte boundaries */
};
extern struct list_head pci_root_buses;	/* list of all known PCI buses */

extern int raw_pci_read(unsigned int domain, unsigned int bus,
	unsigned int devfn, int reg, int len, u32 *val);
extern int raw_pci_write(unsigned int domain, unsigned int bus,
	unsigned int devfn, int reg, int len, u32 val);
void pci_read_bridge_bases(struct pci_bus *child);
extern void pci_sort_breadthfirst(void);
struct pci_bus *pci_find_next_bus(const struct pci_bus *from);

static inline bool pci_is_root_bus(struct pci_bus *pbus)
{
	return !(pbus->parent);
}

static inline int pci_pcie_cap(struct pci_dev *dev)
{
	return dev->pcie_cap;
}

static inline bool pci_is_pcie(struct pci_dev *dev)
{
	return !!pci_pcie_cap(dev);
}

/**
 * pci_ari_enabled - query ARI forwarding status
 * @bus: the PCI bus
 *
 * Returns 1 if ARI forwarding is enabled, or 0 if not enabled;
 */
static inline int pci_ari_enabled(struct pci_bus *bus)
{
	return bus->self && bus->self->ari_enabled;
}

#ifdef CONFIG_PCI_MSI
static inline bool pci_dev_msi_enabled(struct pci_dev *pci_dev)
{
	return pci_dev->msi_enabled || pci_dev->msix_enabled;
}
#else
static inline bool pci_dev_msi_enabled(struct pci_dev *pci_dev) { return false; }
#endif

/* setup-res.c */
int pci_enable_resources(struct pci_dev *, int mask);

/* access.c */
int pci_bus_read_config_byte(struct pci_bus *bus, unsigned int devfn, int where, u8 *val);
int pci_bus_read_config_word(struct pci_bus *bus, unsigned int devfn, int where, u16 *val);
int pci_bus_read_config_dword(struct pci_bus *bus, unsigned int devfn, int where, u32 *val);
int pci_bus_write_config_byte(struct pci_bus *bus, unsigned int devfn, int where, u8 val);
int pci_bus_write_config_word(struct pci_bus *bus, unsigned int devfn, int where, u16 val);
int pci_bus_write_config_dword(struct pci_bus *bus, unsigned int devfn, int where, u32 val);

/* probe.c */
struct device;
struct pci_bus * __devinit pci_scan_root_bus(struct device *parent, int bus, struct pci_ops *ops, void *sysdata, struct list_head *resources);
int pci_dev_present(const struct pci_device_id *ids);
int pci_bus_insert_busn_res(struct pci_bus *b, int bus, int busmax);
unsigned int pci_scan_child_bus(struct pci_bus *bus);
int pci_bus_update_busn_res_end(struct pci_bus *b, int bus_max);
int pci_cfg_space_size(struct pci_dev *dev);
int pci_cfg_space_size_ext(struct pci_dev *dev);

/* search.c */
struct pci_dev *pci_get_domain_bus_and_slot(int domain, unsigned int bus, unsigned int devfn);
static inline struct pci_dev *pci_get_bus_and_slot(unsigned int bus,
	unsigned int devfn)
{
	return pci_get_domain_bus_and_slot(0, bus, devfn);
}
struct pci_dev * pci_get_slot(struct pci_bus *bus, unsigned int devfn);
void pci_bus_lock();	/* 总线加锁、解锁，用以互斥处理一些总线设备的读写 */
void pci_bus_unlock();
struct pci_dev *pci_get_device(unsigned int vendor, unsigned int device,
				struct pci_dev *from);
struct pci_dev *pci_get_subsys(unsigned int vendor, unsigned int device,
				unsigned int ss_vendor, unsigned int ss_device,
				struct pci_dev *from);
extern struct pci_bus *pci_find_bus(int domain, int busnr);


/* pci.c */
struct pci_platform_pm_ops {
	bool (*is_manageable)(struct pci_dev *dev);
	int (*set_state)(struct pci_dev *dev, pci_power_t state);
	pci_power_t (*choose_state)(struct pci_dev *dev);
	bool (*can_wakeup)(struct pci_dev *dev);
	int (*sleep_wake)(struct pci_dev *dev, bool enable);
	int (*run_wake)(struct pci_dev *dev, bool enable);
};
extern int pci_set_platform_pm(struct pci_platform_pm_ops *ops);
int pci_find_capability(struct pci_dev *dev, int cap);
int pci_find_ext_capability(struct pci_dev *dev, int cap);
void pci_wakeup_bus(struct pci_bus *bus);
int pci_resource_bar(struct pci_dev *dev, int resno, enum pci_bar_type *type);

/* bus.c */
#define PCI_SUBTRACTIVE_DECODE	0x1
struct pci_bus_resource {
	struct list_head list;
	struct resource *res;
	unsigned int flags;
};
void pci_add_resource(struct list_head *resources, struct resource *res);
void pci_add_resource_offset(struct list_head *resources, struct resource *res, resource_size_t offset);
void pci_free_resource_list(struct list_head *resources);
void pci_bus_add_resource(struct pci_bus *bus, struct resource *res,
						  unsigned int flags);
void pci_bus_remove_resources(struct pci_bus *bus);
struct resource *pci_bus_resource_n(const struct pci_bus *bus, int n);
void pci_bus_add_devices(const struct pci_bus *bus);
#define pci_bus_for_each_resource(bus, res, i)				\
	for (i = 0;							\
	(res = pci_bus_resource_n(bus, i)) || i < PCI_BRIDGE_RESOURCE_NUM; \
	i++)
void pci_walk_bus(struct pci_bus *top, int (*cb)(struct pci_dev *, void *),
				  void *userdata);

/* pci-driver.c */
extern struct pci_dev *pci_dev_get(struct pci_dev *dev);
extern void pci_dev_put(struct pci_dev *dev);
const struct pci_device_id *pci_match_id(const struct pci_device_id *ids,
					 struct pci_dev *dev);

/* setup-res.c */
void pci_update_resource(struct pci_dev *dev, int resno);

/* host-bridge.c */
struct pci_host_bridge_window {
	struct list_head list;
	struct resource *res;		/* host bridge aperture (CPU address) */
	resource_size_t offset;		/* bus address + offset = CPU address */
};

struct pci_host_bridge {
	struct pci_bus *bus;		/* root bus */
	struct list_head windows;	/* pci_host_bridge_windows */
	void (*release_fn)(struct pci_host_bridge *);
	void *release_data;
};
#define	to_pci_host_bridge(n) n /* 由于我们直接把host_bridge 挂接到了bus->bridge,因此这里直接返回 */
void pcibios_bus_to_resource(struct pci_dev *dev, struct resource *res,
							 			     struct pci_bus_region *region);
void pcibios_resource_to_bus(struct pci_dev *dev, struct pci_bus_region *region,
											 struct resource *res);
/* quirks.c */
enum pci_fixup_pass {
	pci_fixup_early,	/* Before probing BARs */
	pci_fixup_header,	/* After reading configuration header */
	pci_fixup_final,	/* Final phase of device fixups */
	pci_fixup_enable,	/* pci_enable_device() time */
	pci_fixup_resume,	/* pci_device_resume() */
	pci_fixup_suspend,	/* pci_device_suspend */
	pci_fixup_resume_early, /* pci_device_resume_early() */
};

#ifdef CONFIG_PCI_QUIRKS
struct pci_dev *pci_get_dma_source(struct pci_dev *dev);
int pci_dev_specific_acs_enabled(struct pci_dev *dev, u16 acs_flags);
#else
extern struct list_head quirk_final;
void pci_fixup_device(enum pci_fixup_pass pass, struct pci_dev *dev);

static inline struct pci_dev *pci_get_dma_source(struct pci_dev *dev)
{
	return pci_dev_get(dev);
}
static inline int pci_dev_specific_acs_enabled(struct pci_dev *dev,
											   u16 acs_flags)
{
	return -ENOTTY;
}
#endif

/* arch */
void pcibios_fixup_bus(struct pci_bus *);
int  pcibios_enable_device(struct pci_dev *, int mask);
void pcibios_disable_device(struct pci_dev *dev);

/*
 * PCI domain support.  Sometimes called PCI segment (eg by ACPI),
 * a PCI domain is defined to be a set of PCI busses which share
 * configuration space.
 */
#ifdef CONFIG_PCI_DOMAINS
extern int pci_domains_supported;
#else
enum { pci_domains_supported = 0 };
static inline int pci_domain_nr(struct pci_bus *bus)
{
	return 0;
}

static inline int pci_proc_domain(struct pci_bus *bus)
{
	return 0;
}
#endif /* CONFIG_PCI_DOMAINS */

#ifdef CONFIG_PCI_MMCONFIG
extern void __init pci_mmcfg_early_init(void);
extern void __init pci_mmcfg_late_init(void);
#else
static inline void pci_mmcfg_early_init(void) { }
static inline void pci_mmcfg_late_init(void) { }
#endif

#ifndef CONFIG_PCI_IOV
static inline int pci_iov_bus_range(struct pci_bus *bus)
{
	return 0;
}
static inline int pci_iov_resource_bar(struct pci_dev *dev, int resno,
									   enum pci_bar_type *type)
{
	return 0;
}

#endif /* CONFIG_PCI_IOV */

#ifdef CONFIG_PCIEASPM
#else
static inline void pcie_aspm_init_link_state(struct pci_dev *pdev)
{
}
static inline void pcie_aspm_pm_state_change(struct pci_dev *pdev)
{
}
static inline void pcie_aspm_powersave_config_link(struct pci_dev *pdev)
{
}

#endif

#ifdef CONFIG_OF
#else
static inline void pci_set_of_node(struct pci_dev *dev) { }
#endif

static inline int pci_no_d1d2(struct pci_dev *dev)
{
	unsigned int parent_dstates = 0;

	if (dev->bus->self)
		parent_dstates = dev->bus->self->no_d1d2;
	return (dev->no_d1d2 || parent_dstates);
}

/**
 * pci_match_one_device - Tell if a PCI device structure has a matching
 *                        PCI device id structure
 * @id: single PCI device id structure to match
 * @dev: the PCI device structure to match against
 *
 * Returns the matching pci_device_id structure or %NULL if there is no match.
 */
static inline const struct pci_device_id *
	pci_match_one_device(const struct pci_device_id *id, const struct pci_dev *dev)
{
	if ((id->vendor == PCI_ANY_ID || id->vendor == dev->vendor) &&
	    (id->device == PCI_ANY_ID || id->device == dev->device) &&
	    (id->subvendor == PCI_ANY_ID || id->subvendor == dev->subsystem_vendor) &&
	    (id->subdevice == PCI_ANY_ID || id->subdevice == dev->subsystem_device) &&
	    !((id->class ^ dev->class) & id->class_mask))
		return id;
	return NULL;
}

static inline const char *pci_name(const struct pci_dev *pdev)
{
	return do_get_device_name((device_t)pdev);
}

#define pci_dev_b(n) list_entry(n, struct pci_dev, bus_list)
#define pci_bus_b(n)	list_entry(n, struct pci_bus, node)
#define for_each_pci_dev(d) while ((d = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, d)) != NULL)

#endif 

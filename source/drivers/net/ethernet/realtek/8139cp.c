/**
	8138 C+ Íø¿¨Çý¶¯
*/
#include <errno.h>

#include <ddk/log.h>
#include <ddk/resource.h>
#include <ddk/dma.h>
#include <ddk/byteorder.h>

#include <ddk/pci/pci.h>
#include <ddk/pci/global_ids.h>

#include <ddk/compatible_io.h>
#include <ddk/compatible.h>
#include <ddk/net/etherdevice.h>
#include <ddk/net/mii.h>
#include <ddk/net/ethtool.h>

#define DRV_NAME		"8139cp"
static int debug = -1;

#define ____cacheline_aligned __attribute__((__aligned__(1<<6)))

#define CP_DEF_MSG_ENABLE	(NETIF_MSG_DRV		| \
				 NETIF_MSG_PROBE 	| \
				 NETIF_MSG_LINK)
#define CP_NUM_STATS		14	/* struct cp_dma_stats, plus one */
#define CP_STATS_SIZE		64	/* size in bytes of DMA stats block */
#define CP_REGS_SIZE		(0xff + 1)
#define CP_REGS_VER		1		/* version 1 */
#define CP_RX_RING_SIZE		64
#define CP_TX_RING_SIZE		64
#define CP_RING_BYTES		\
		((sizeof(struct cp_desc) * CP_RX_RING_SIZE) +	\
		 (sizeof(struct cp_desc) * CP_TX_RING_SIZE) +	\
		 CP_STATS_SIZE)
#define NEXT_TX(N)		(((N) + 1) & (CP_TX_RING_SIZE - 1))
#define NEXT_RX(N)		(((N) + 1) & (CP_RX_RING_SIZE - 1))
#define TX_BUFFS_AVAIL(CP)					\
	(((CP)->tx_tail <= (CP)->tx_head) ?			\
	  (CP)->tx_tail + (CP_TX_RING_SIZE - 1) - (CP)->tx_head :	\
	  (CP)->tx_tail - (CP)->tx_head - 1)

#define PKT_BUF_SZ		1536	/* Size of each temporary Rx buffer.*/
#define CP_INTERNAL_PHY		32

/* The following settings are log_2(bytes)-4:  0 == 16 bytes .. 6==1024, 7==end of packet. */
#define RX_FIFO_THRESH		5	/* Rx buffer level before first PCI xfer.  */
#define RX_DMA_BURST		4	/* Maximum PCI burst, '4' is 256 */
#define TX_DMA_BURST		6	/* Maximum PCI burst, '6' is 1024 */
#define TX_EARLY_THRESH		256	/* Early Tx threshold, in bytes */

/* Time in jiffies before concluding the transmitter is hung. */
#define TX_TIMEOUT		(6*HZ)

/* hardware minimum and maximum for a single frame's data payload */
#define CP_MIN_MTU		60	/* TODO: allow lower, but pad */
#define CP_MAX_MTU		4096

enum {
	/* NIC register offsets */
	MAC0		= 0x00,	/* Ethernet hardware address. */
	MAR0		= 0x08,	/* Multicast filter. */
	StatsAddr	= 0x10,	/* 64-bit start addr of 64-byte DMA stats blk */
	TxRingAddr	= 0x20, /* 64-bit start addr of Tx ring */
	HiTxRingAddr	= 0x28, /* 64-bit start addr of high priority Tx ring */
	Cmd		= 0x37, /* Command register */
	IntrMask	= 0x3C, /* Interrupt mask */
	IntrStatus	= 0x3E, /* Interrupt status */
	TxConfig	= 0x40, /* Tx configuration */
	ChipVersion	= 0x43, /* 8-bit chip version, inside TxConfig */
	RxConfig	= 0x44, /* Rx configuration */
	RxMissed	= 0x4C,	/* 24 bits valid, write clears */
	Cfg9346		= 0x50, /* EEPROM select/control; Cfg reg [un]lock */
	Config1		= 0x52, /* Config1 */
	Config3		= 0x59, /* Config3 */
	Config4		= 0x5A, /* Config4 */
	MultiIntr	= 0x5C, /* Multiple interrupt select */
	BasicModeCtrl	= 0x62,	/* MII BMCR */
	BasicModeStatus	= 0x64, /* MII BMSR */
	NWayAdvert	= 0x66, /* MII ADVERTISE */
	NWayLPAR	= 0x68, /* MII LPA */
	NWayExpansion	= 0x6A, /* MII Expansion */
	Config5		= 0xD8,	/* Config5 */
	TxPoll		= 0xD9,	/* Tell chip to check Tx descriptors for work */
	RxMaxSize	= 0xDA, /* Max size of an Rx packet (8169 only) */
	CpCmd		= 0xE0, /* C+ Command register (C+ mode only) */
	IntrMitigate	= 0xE2,	/* rx/tx interrupt mitigation control */
	RxRingAddr	= 0xE4, /* 64-bit start addr of Rx ring */
	TxThresh	= 0xEC, /* Early Tx threshold */
	OldRxBufAddr	= 0x30, /* DMA address of Rx ring buffer (C mode) */
	OldTSD0		= 0x10, /* DMA address of first Tx desc (C mode) */

	/* Tx and Rx status descriptors */
	DescOwn		= (1 << 31), /* Descriptor is owned by NIC */
	RingEnd		= (1 << 30), /* End of descriptor ring */
	FirstFrag	= (1 << 29), /* First segment of a packet */
	LastFrag	= (1 << 28), /* Final segment of a packet */
	LargeSend	= (1 << 27), /* TCP Large Send Offload (TSO) */
	MSSShift	= 16,	     /* MSS value position */
	MSSMask		= 0xfff,     /* MSS value: 11 bits */
	TxError		= (1 << 23), /* Tx error summary */
	RxError		= (1 << 20), /* Rx error summary */
	IPCS		= (1 << 18), /* Calculate IP checksum */
	UDPCS		= (1 << 17), /* Calculate UDP/IP checksum */
	TCPCS		= (1 << 16), /* Calculate TCP/IP checksum */
	TxVlanTag	= (1 << 17), /* Add VLAN tag */
	RxVlanTagged	= (1 << 16), /* Rx VLAN tag available */
	IPFail		= (1 << 15), /* IP checksum failed */
	UDPFail		= (1 << 14), /* UDP/IP checksum failed */
	TCPFail		= (1 << 13), /* TCP/IP checksum failed */
	NormalTxPoll	= (1 << 6),  /* One or more normal Tx packets to send */
	PID1		= (1 << 17), /* 2 protocol id bits:  0==non-IP, */
	PID0		= (1 << 16), /* 1==UDP/IP, 2==TCP/IP, 3==IP */
	RxProtoTCP	= 1,
	RxProtoUDP	= 2,
	RxProtoIP	= 3,
	TxFIFOUnder	= (1 << 25), /* Tx FIFO underrun */
	TxOWC		= (1 << 22), /* Tx Out-of-window collision */
	TxLinkFail	= (1 << 21), /* Link failed during Tx of packet */
	TxMaxCol	= (1 << 20), /* Tx aborted due to excessive collisions */
	TxColCntShift	= 16,	     /* Shift, to get 4-bit Tx collision cnt */
	TxColCntMask	= 0x01 | 0x02 | 0x04 | 0x08, /* 4-bit collision count */
	RxErrFrame	= (1 << 27), /* Rx frame alignment error */
	RxMcast		= (1 << 26), /* Rx multicast packet rcv'd */
	RxErrCRC	= (1 << 18), /* Rx CRC error */
	RxErrRunt	= (1 << 19), /* Rx error, packet < 64 bytes */
	RxErrLong	= (1 << 21), /* Rx error, packet > 4096 bytes */
	RxErrFIFO	= (1 << 22), /* Rx error, FIFO overflowed, pkt bad */

	/* StatsAddr register */
	DumpStats	= (1 << 3),  /* Begin stats dump */

	/* RxConfig register */
	RxCfgFIFOShift	= 13,	     /* Shift, to get Rx FIFO thresh value */
	RxCfgDMAShift	= 8,	     /* Shift, to get Rx Max DMA value */
	AcceptErr	= 0x20,	     /* Accept packets with CRC errors */
	AcceptRunt	= 0x10,	     /* Accept runt (<64 bytes) packets */
	AcceptBroadcast	= 0x08,	     /* Accept broadcast packets */
	AcceptMulticast	= 0x04,	     /* Accept multicast packets */
	AcceptMyPhys	= 0x02,	     /* Accept pkts with our MAC as dest */
	AcceptAllPhys	= 0x01,	     /* Accept all pkts w/ physical dest */

	/* IntrMask / IntrStatus registers */
	PciErr		= (1 << 15), /* System error on the PCI bus */
	TimerIntr	= (1 << 14), /* Asserted when TCTR reaches TimerInt value */
	LenChg		= (1 << 13), /* Cable length change */
	SWInt		= (1 << 8),  /* Software-requested interrupt */
	TxEmpty		= (1 << 7),  /* No Tx descriptors available */
	RxFIFOOvr	= (1 << 6),  /* Rx FIFO Overflow */
	LinkChg		= (1 << 5),  /* Packet underrun, or link change */
	RxEmpty		= (1 << 4),  /* No Rx descriptors available */
	TxErr		= (1 << 3),  /* Tx error */
	TxOK		= (1 << 2),  /* Tx packet sent */
	RxErr		= (1 << 1),  /* Rx error */
	RxOK		= (1 << 0),  /* Rx packet received */
	IntrResvd	= (1 << 10), /* reserved, according to RealTek engineers,
					but hardware likes to raise it */

	IntrAll		= PciErr | TimerIntr | LenChg | SWInt | TxEmpty |
			  RxFIFOOvr | LinkChg | RxEmpty | TxErr | TxOK |
			  RxErr | RxOK | IntrResvd,

	/* C mode command register */
	CmdReset	= (1 << 4),  /* Enable to reset; self-clearing */
	RxOn		= (1 << 3),  /* Rx mode enable */
	TxOn		= (1 << 2),  /* Tx mode enable */

	/* C+ mode command register */
	RxVlanOn	= (1 << 6),  /* Rx VLAN de-tagging enable */
	RxChkSum	= (1 << 5),  /* Rx checksum offload enable */
	PCIDAC		= (1 << 4),  /* PCI Dual Address Cycle (64-bit PCI) */
	PCIMulRW	= (1 << 3),  /* Enable PCI read/write multiple */
	CpRxOn		= (1 << 1),  /* Rx mode enable */
	CpTxOn		= (1 << 0),  /* Tx mode enable */

	/* Cfg9436 EEPROM control register */
	Cfg9346_Lock	= 0x00,	     /* Lock ConfigX/MII register access */
	Cfg9346_Unlock	= 0xC0,	     /* Unlock ConfigX/MII register access */

	/* TxConfig register */
	IFG		= (1 << 25) | (1 << 24), /* standard IEEE interframe gap */
	TxDMAShift	= 8,	     /* DMA burst value (0-7) is shift this many bits */

	/* Early Tx Threshold register */
	TxThreshMask	= 0x3f,	     /* Mask bits 5-0 */
	TxThreshMax	= 2048,	     /* Max early Tx threshold */

	/* Config1 register */
	DriverLoaded	= (1 << 5),  /* Software marker, driver is loaded */
	LWACT           = (1 << 4),  /* LWAKE active mode */
	PMEnable	= (1 << 0),  /* Enable various PM features of chip */

	/* Config3 register */
	PARMEnable	= (1 << 6),  /* Enable auto-loading of PHY parms */
	MagicPacket     = (1 << 5),  /* Wake up when receives a Magic Packet */
	LinkUp          = (1 << 4),  /* Wake up when the cable connection is re-established */

	/* Config4 register */
	LWPTN           = (1 << 1),  /* LWAKE Pattern */
	LWPME           = (1 << 4),  /* LANWAKE vs PMEB */

	/* Config5 register */
	BWF             = (1 << 6),  /* Accept Broadcast wakeup frame */
	MWF             = (1 << 5),  /* Accept Multicast wakeup frame */
	UWF             = (1 << 4),  /* Accept Unicast wakeup frame */
	LANWake         = (1 << 1),  /* Enable LANWake signal */
	PMEStatus	= (1 << 0),  /* PME status can be reset by PCI RST# */

	cp_norx_intr_mask = PciErr | LinkChg | TxOK | TxErr | TxEmpty,
	cp_rx_intr_mask = RxOK | RxErr | RxEmpty | RxFIFOOvr,
	cp_intr_mask = cp_rx_intr_mask | cp_norx_intr_mask,
};

static const unsigned int cp_rx_config =
	  (RX_FIFO_THRESH << RxCfgFIFOShift) |
	  (RX_DMA_BURST << RxCfgDMAShift);

struct cp_desc {
	__le32		opts1;
	__le32		opts2;
	__le64		addr;
};

struct cp_dma_stats {
	__le64			tx_ok;
	__le64			rx_ok;
	__le64			tx_err;
	__le32			rx_err;
	__le16			rx_fifo;
	__le16			frame_align;
	__le32			tx_ok_1col;
	__le32			tx_ok_mcol;
	__le64			rx_ok_phys;
	__le64			rx_ok_bcast;
	__le32			rx_ok_mcast;
	__le16			tx_abort;
	__le16			tx_underrun;
} __packed;

struct cp_extra_stats {
	unsigned long		rx_frags;
};

struct cp_private {
	void			__iomem *regs;
	struct net_device	*dev;
	spinlock_t		lock;
	u32			msg_enable;

//	struct napi_struct	napi;

	struct pci_dev		*pdev;
	u32			rx_config;
	u16			cpcmd;

	struct cp_extra_stats	cp_stats;

	unsigned		rx_head		____cacheline_aligned;
	unsigned		rx_tail;
	struct cp_desc		*rx_ring;
	struct sk_buff		*rx_skb[CP_RX_RING_SIZE];

	unsigned		tx_head		____cacheline_aligned;
	unsigned		tx_tail;
	struct cp_desc		*tx_ring;
	struct sk_buff		*tx_skb[CP_TX_RING_SIZE];

	unsigned		rx_buf_sz;
	unsigned		wol_enabled : 1; /* Is Wake-on-LAN enabled? */

	dma_addr_t		ring_dma;

	struct mii_if_info	mii_if;
};

#define cpr8(reg)	readb(cp->regs + (reg))
#define cpr16(reg)	readw(cp->regs + (reg))
#define cpr32(reg)	readl(cp->regs + (reg))
#define cpw8(reg,val)	writeb((val), cp->regs + (reg))
#define cpw16(reg,val)	writew((val), cp->regs + (reg))
#define cpw32(reg,val)	writel((val), cp->regs + (reg))
#define cpw8_f(reg,val) do {			\
	writeb((val), cp->regs + (reg));	\
	readb(cp->regs + (reg));		\
	} while (0)
#define cpw16_f(reg,val) do {			\
	writew((val), cp->regs + (reg));	\
	readw(cp->regs + (reg));		\
	} while (0)
#define cpw32_f(reg,val) do {			\
	writel((val), cp->regs + (reg));	\
	readl(cp->regs + (reg));		\
	} while (0)


static DEFINE_PCI_DEVICE_TABLE(cp_pci_tbl) = {
	{ PCI_DEVICE(PCI_VENDOR_ID_REALTEK,	PCI_DEVICE_ID_REALTEK_8139), },
	{ PCI_DEVICE(PCI_VENDOR_ID_TTTECH,	PCI_DEVICE_ID_TTTECH_MC322), },
	{ },
};

static struct {
	const char str[ETH_GSTRING_LEN];
} ethtool_stats_keys[] = {
	{ "tx_ok" },
	{ "rx_ok" },
	{ "tx_err" },
	{ "rx_err" },
	{ "rx_fifo" },
	{ "frame_align" },
	{ "tx_ok_1col" },
	{ "tx_ok_mcol" },
	{ "rx_ok_phys" },
	{ "rx_ok_bcast" },
	{ "rx_ok_mcast" },
	{ "tx_abort" },
	{ "tx_underrun" },
	{ "rx_frags" },
};


static inline void cp_set_rxbufsize (struct cp_private *cp)
{
	unsigned int mtu = cp->dev->mtu;

	if (mtu > ETH_DATA_LEN)
		/* MTU + ethernet header + FCS + optional VLAN tag */
		cp->rx_buf_sz = mtu + ETH_HLEN + 8;
	else
		cp->rx_buf_sz = PKT_BUF_SZ;
}

static const char mii_2_8139_map[8] = {
	BasicModeCtrl,
	BasicModeStatus,
	0,
	0,
	NWayAdvert,
	NWayLPAR,
	NWayExpansion,
	0
};

static int mdio_read(struct net_device *dev, int phy_id, int location)
{
	struct cp_private *cp = netdev_priv(dev);

	return location < 8 && mii_2_8139_map[location] ?
	       readw(cp->regs + mii_2_8139_map[location]) : 0;
}


static void mdio_write(struct net_device *dev, int phy_id, int location,
		       int value)
{
	struct cp_private *cp = netdev_priv(dev);

	if (location == 0) {
		cpw8(Cfg9346, Cfg9346_Unlock);
		cpw16(BasicModeCtrl, value);
		cpw8(Cfg9346, Cfg9346_Lock);
	} else if (location < 8 && mii_2_8139_map[location])
		cpw16(mii_2_8139_map[location], value);
}

static void cp_stop_hw (struct cp_private *cp)
{
	cpw16(IntrStatus, ~(cpr16(IntrStatus)));
	cpw16_f(IntrMask, 0);
	cpw8(Cmd, 0);
	cpw16_f(CpCmd, 0);
	cpw16_f(IntrStatus, ~(cpr16(IntrStatus)));
	
	cp->rx_tail = 0;
	cp->tx_head = cp->tx_tail = 0;
	
	netdev_reset_queue(cp->dev);
}


/* Serial EEPROM section. */

/*  EEPROM_Ctrl bits. */
#define EE_SHIFT_CLK	0x04	/* EEPROM shift clock. */
#define EE_CS			0x08	/* EEPROM chip select. */
#define EE_DATA_WRITE	0x02	/* EEPROM chip data in. */
#define EE_WRITE_0		0x00
#define EE_WRITE_1		0x02
#define EE_DATA_READ	0x01	/* EEPROM chip data out. */
#define EE_ENB			(0x80 | EE_CS)

/* Delay between EEPROM clock transitions.
 No extra delay is needed with 33Mhz PCI, but 66Mhz may change this.
 */

#define eeprom_delay()	readb(ee_addr)

/* The EEPROM commands include the alway-set leading bit. */
#define EE_EXTEND_CMD	(4)
#define EE_WRITE_CMD	(5)
#define EE_READ_CMD		(6)
#define EE_ERASE_CMD	(7)

#define EE_EWDS_ADDR	(0)
#define EE_WRAL_ADDR	(1)
#define EE_ERAL_ADDR	(2)
#define EE_EWEN_ADDR	(3)

#define CP_EEPROM_MAGIC PCI_DEVICE_ID_REALTEK_8139

static void eeprom_cmd_start(void __iomem *ee_addr)
{
	writeb (EE_ENB & ~EE_CS, ee_addr);
	writeb (EE_ENB, ee_addr);
	eeprom_delay ();
}

static void eeprom_cmd(void __iomem *ee_addr, int cmd, int cmd_len)
{
	int i;
	
	/* Shift the command bits out. */
	for (i = cmd_len - 1; i >= 0; i--) {
		int dataval = (cmd & (1 << i)) ? EE_DATA_WRITE : 0;
		writeb (EE_ENB | dataval, ee_addr);
		eeprom_delay ();
		writeb (EE_ENB | dataval | EE_SHIFT_CLK, ee_addr);
		eeprom_delay ();
	}
	writeb (EE_ENB, ee_addr);
	eeprom_delay ();
}

static void eeprom_cmd_end(void __iomem *ee_addr)
{
	writeb(0, ee_addr);
	eeprom_delay ();
}

static void eeprom_extend_cmd(void __iomem *ee_addr, int extend_cmd,
							  int addr_len)
{
	int cmd = (EE_EXTEND_CMD << addr_len) | (extend_cmd << (addr_len - 2));
	
	eeprom_cmd_start(ee_addr);
	eeprom_cmd(ee_addr, cmd, 3 + addr_len);
	eeprom_cmd_end(ee_addr);
}

static u16 read_eeprom (void __iomem *ioaddr, int location, int addr_len)
{
	int i;
	u16 retval = 0;
	void __iomem *ee_addr = ioaddr + Cfg9346;
	int read_cmd = location | (EE_READ_CMD << addr_len);
	
	eeprom_cmd_start(ee_addr);
	eeprom_cmd(ee_addr, read_cmd, 3 + addr_len);
	
	for (i = 16; i > 0; i--) {
		writeb (EE_ENB | EE_SHIFT_CLK, ee_addr);
		eeprom_delay ();
		retval =
		(retval << 1) | ((readb (ee_addr) & EE_DATA_READ) ? 1 :
						 0);
		writeb (EE_ENB, ee_addr);
		eeprom_delay ();
	}
	
	eeprom_cmd_end(ee_addr);
	
	return retval;
}

static void write_eeprom(void __iomem *ioaddr, int location, u16 val,
						 int addr_len)
{
	int i;
	void __iomem *ee_addr = ioaddr + Cfg9346;
	int write_cmd = location | (EE_WRITE_CMD << addr_len);
	
	eeprom_extend_cmd(ee_addr, EE_EWEN_ADDR, addr_len);
	
	eeprom_cmd_start(ee_addr);
	eeprom_cmd(ee_addr, write_cmd, 3 + addr_len);
	eeprom_cmd(ee_addr, val, 16);
	eeprom_cmd_end(ee_addr);
	
	eeprom_cmd_start(ee_addr);
	for (i = 0; i < 20000; i++)
		if (readb(ee_addr) & EE_DATA_READ)
			break;
	eeprom_cmd_end(ee_addr);
	
	eeprom_extend_cmd(ee_addr, EE_EWDS_ADDR, addr_len);
}

static int cp_init_one (struct pci_dev *pdev, const struct pci_device_id *ent)
{
	int rc;
	struct net_device *dev;
	struct cp_private *cp;
	void __iomem *regs;
	resource_size_t pciaddr;
	unsigned int addr_len, i, pci_using_dac;

	
	/* May be old 8139 */
	if (pdev->vendor == PCI_VENDOR_ID_REALTEK &&
		pdev->device == PCI_DEVICE_ID_REALTEK_8139 && pdev->revision < 0x20) {
			dev_info(&pdev->dev,
				"This (id %04x:%04x rev %02x) is not an 8139C+ compatible chip, use 8139too\n",
				pdev->vendor, pdev->device, pdev->revision);
			return -ENODEV;
	}

	dev = alloc_etherdev(sizeof(struct cp_private));
	if (!dev)
		return -ENOMEM;

	cp = netdev_priv(dev);
	cp->pdev = pdev;
	cp->dev = dev;
	cp->msg_enable = (debug < 0 ? CP_DEF_MSG_ENABLE : debug);
	spin_lock_init (&cp->lock);
	cp->mii_if.dev = dev;
	cp->mii_if.mdio_read = mdio_read;
	cp->mii_if.mdio_write = mdio_write;
	cp->mii_if.phy_id = CP_INTERNAL_PHY;
	cp->mii_if.phy_id_mask = 0x1f;
	cp->mii_if.reg_num_mask = 0x1f;
	cp_set_rxbufsize(cp);
	
	/* Enable the pci device */
	rc = pci_enable_device(pdev);
	if (rc)
		goto err_out_free;

	rc = pci_set_mwi(pdev);
	if (rc)
		goto err_out_disable;
	
	rc = pci_request_regions(pdev, DRV_NAME);
	if (rc)
		goto err_out_mwi;
	
	pciaddr = pci_resource_start(pdev, 1);
	if (!pciaddr) {
		rc = -EIO;
		dev_err(&pdev->dev, "no MMIO resource\n");
		goto err_out_res;
	}
	if (pci_resource_len(pdev, 1) < CP_REGS_SIZE) {
		rc = -EIO;
		dev_err(&pdev->dev, "MMIO resource (%llx) too small\n",
				(unsigned long long)pci_resource_len(pdev, 1));
		goto err_out_res;
	}
	
	/* Configure DMA attributes. */
	if ((sizeof(dma_addr_t) > 4) &&
	    !pci_set_consistent_dma_mask(pdev, DMA_BIT_MASK(64)) &&
	    !pci_set_dma_mask(pdev, DMA_BIT_MASK(64))) {
		pci_using_dac = 1;
	} else {
		pci_using_dac = 0;
		
		rc = pci_set_dma_mask(pdev, DMA_BIT_MASK(32));
		if (rc) {
			dev_err(&pdev->dev,
					"No usable DMA configuration, aborting\n");
			goto err_out_res;
		}
		rc = pci_set_consistent_dma_mask(pdev, DMA_BIT_MASK(32));
		if (rc) {
			dev_err(&pdev->dev,
					"No usable consistent DMA configuration, aborting\n");
			goto err_out_res;
		}
	}
	
	cp->cpcmd = (pci_using_dac ? PCIDAC : 0) |
	PCIMulRW | RxChkSum | CpRxOn | CpTxOn;
	
	dev->features |= NETIF_F_RXCSUM;
	dev->hw_features |= NETIF_F_RXCSUM;

	regs = ioremap(pciaddr, CP_REGS_SIZE);
	if (!regs) {
		rc = -EIO;
		dev_err(&pdev->dev, "Cannot map PCI MMIO (%Lx@%Lx)\n",
				(unsigned long long)pci_resource_len(pdev, 1),
				(unsigned long long)pciaddr);
		goto err_out_res;
	}
	cp->regs = regs;

	cp_stop_hw(cp);

	/* read MAC address from EEPROM */
	addr_len = read_eeprom (regs, 0, 8) == 0x8129 ? 8 : 6;
	for (i = 0; i < 3; i++)
		((__le16 *) (dev->dev_addr))[i] =
		cpu_to_le16(read_eeprom (regs, i + 7, addr_len));
	memcpy(dev->perm_addr, dev->dev_addr, dev->addr_len);
	printk("dev->addr_len = %d, MAC %02x:%02x:%02x:%02x:%02x:%02x.\n",dev->addr_len,
																dev->perm_addr[0], dev->perm_addr[1], dev->perm_addr[2], dev->perm_addr[3], dev->perm_addr[4], dev->perm_addr[5]);
#if 0
	dev->netdev_ops = &cp_netdev_ops;
	netif_napi_add(dev, &cp->napi, cp_rx_poll, 16);
	dev->ethtool_ops = &cp_ethtool_ops;
	dev->watchdog_timeo = TX_TIMEOUT;
	
	dev->features |= NETIF_F_HW_VLAN_TX | NETIF_F_HW_VLAN_RX;
	
	if (pci_using_dac)
		dev->features |= NETIF_F_HIGHDMA;
	
	/* disabled by default until verified */
	dev->hw_features |= NETIF_F_SG | NETIF_F_IP_CSUM | NETIF_F_TSO |
	NETIF_F_HW_VLAN_TX | NETIF_F_HW_VLAN_RX;
	dev->vlan_features = NETIF_F_SG | NETIF_F_IP_CSUM | NETIF_F_TSO |
	NETIF_F_HIGHDMA;
	
	rc = register_netdev(dev);
	if (rc)
		goto err_out_iomap;
	
	netdev_info(dev, "RTL-8139C+ at 0x%p, %pM, IRQ %d\n",
				regs, dev->dev_addr, pdev->irq);
	
	pci_set_drvdata(pdev, dev);
	
	/* enable busmastering and memory-write-invalidate */
	pci_set_master(pdev);
	
	if (cp->wol_enabled)
		cp_set_d3_state (cp);
#endif
	return 0;
	
err_out_iomap:
	iounmap(regs);
err_out_res:
	pci_release_regions(pdev);
err_out_mwi:
	pci_clear_mwi(pdev);
err_out_disable:
	pci_disable_device(pdev);
err_out_free:
	free_netdev(dev);
	return rc;
}

static void cp_remove_one (struct pci_dev *pdev)
{

}

struct ddk_pci_driver cp_driver = {
	.name			= DRV_NAME,
	.id_table		= cp_pci_tbl,
	.probe			= cp_init_one,
	.remove			= cp_remove_one,
};

static int __init cp_init (void)
{
	return pci_register_driver(&cp_driver);
}

static void cp_exit (void)
{
	pci_unregister_driver (&cp_driver);
}

driver_initcall(cp_init);

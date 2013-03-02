
#define PCI_PROBE_BIOS		0x0001
#define PCI_PROBE_CONF1		0x0002
#define PCI_PROBE_CONF2		0x0004
#define PCI_PROBE_MMCONF	0x0008
#define PCI_PROBE_MASK		0x000f
#define PCI_PROBE_NOEARLY	0x0010

extern unsigned int pci_probe;


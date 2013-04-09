#ifndef DDK_COMPATIBLE_IO_H
#define DDK_COMPATIBLE_IO_H

#include <ddk/log.h>
#include <ddk/compiler.h>

/* IO */
#include <ddk/io.h>
#define inb hal_inb
#define inb_p hal_inb_p

#define inw hal_inw

#define inl hal_inl
#define inl_p hal_inl_p

#define outb hal_outb
#define outb_p hal_outb_p

#define outw hal_outw

#define outl hal_outl
#define outl_p hal_outl_p

#define readl hal_readl
#define writel hal_writel
#define readb hal_readb

static inline unsigned int ioread8(void __iomem *addr)
{
	TODO("");
}

static inline unsigned int ioread32(void __iomem *addr)
{
	return readl(addr);
}


#endif
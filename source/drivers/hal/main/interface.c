#include <types.h>
#include <asm/cpufeature.h>
/************************************************************************/
/* IRQ                                                                  */
/************************************************************************/
#include <ddk/irq.h>
#include <irqflags.h>
 
DLLEXPORT unsigned long hal_local_irq_save()
{
	unsigned long flags;
	local_irq_save(flags);
	return flags;
}

DLLEXPORT void hal_local_irq_restore(unsigned long flags)
{
	local_irq_restore(flags);
}

/************************************************************************/
/* in/out ЦёБо                                                           */
/************************************************************************/
#include <ddk/io.h>
#include <asm/io.h>

DLLEXPORT unsigned char hal_inb(unsigned long port)
{
	return inb(port);
}

DLLEXPORT unsigned char hal_inb_p(unsigned long port)
{
	return inb_p(port);
}

DLLEXPORT unsigned short hal_inw(unsigned long port)
{
	return inw(port);
}

DLLEXPORT unsigned int hal_inl(unsigned long port)
{
	return inl(port);
}

DLLEXPORT unsigned int hal_inl_p(unsigned long port)
{
	return inl_p(port);
}

DLLEXPORT void hal_outb(unsigned char val, unsigned long port)
{
	outb(val, port);
}

DLLEXPORT void hal_outb_p(unsigned char val, unsigned long port)
{
	outb_p(val, port);
}

DLLEXPORT void hal_outw(unsigned short val, unsigned long port)
{
	outw(val, port);
}

DLLEXPORT void hal_outl(unsigned int val, unsigned long port)
{
	outl(val, port);
}

DLLEXPORT void hal_outl_p(unsigned int val, unsigned long port)
{
	outl_p(val, port);
}

DLLEXPORT u32 hal_readl(const volatile void *addr)
{
	return readl(addr);
}

DLLEXPORT unsigned char  hal_readb(const volatile void *addr)
{
	return readb(addr);
}

DLLEXPORT void hal_writel(u32 val, volatile void *addr)
{
	writel(val, addr);
}

/************************************************************************/
/* BARRIER                                                              */
/************************************************************************/
#include <asm/barrier.h>
DLLEXPORT void hal_wmb()
{
	wmb();
}

DLLEXPORT void hal_rmb()
{
	rmb();
}

DLLEXPORT void hal_mb()
{
	mb();
}

#ifndef DDK_COMPATIBLE_IO_H
#define DDK_COMPATIBLE_IO_H

#include <ddk/compiler.h>
#include <ddk/types.h>

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

#define writeb hal_writeb
#define writew hal_writew
#define writel hal_writel
#define readb hal_readb
#define readw hal_readw
#define readl hal_readl

#include <kernel/ke_memory.h>
#define ioremap(__address__, __size__) km_map_physical(__address__, __size__, 0)
#endif
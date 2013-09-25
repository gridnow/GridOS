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

/* DMA */
#define dma_free_coherent(d,s,c,h) ddk_dma_free_attrs(d,s,c,h,NULL)
#define dma_alloc_coherent(d,s,h,f) ddk_dma_alloc_attrs(d,s,h,f,NULL)
#define dma_map_single(d, a, s, r) ddk_dma_map_single_attrs(d, a, s, r, NULL)
#define dma_unmap_single(d, a, s, r) ddk_dma_unmap_single_attrs(d, a, s, r, NULL)

#endif

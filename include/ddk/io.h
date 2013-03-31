/**
*  @defgroup HalIoInstructions
*  @ingroup DDK
*
*  定义了驱动IO操作接口，IO指令在不同平台上是不一样的，因此这里做了屏蔽，同时也兼容了一些传统驱动习惯性编写的inb/outb等类似的接口
*  @{
*/

#ifndef _DDK_IO_H_
#define _DDK_IO_H_

/**
	@brief Read 1 byte from port
*/
unsigned char hal_inb(unsigned long port);

/**
	@brief Read 1 byte from port
*/
unsigned char hal_inb_p(unsigned long port);

/**
	@brief Read 2 bytes from port 
*/
unsigned short hal_inw(unsigned long port);

/**
	@brief Read 4 bytes from port 
*/
unsigned int hal_inl(unsigned long port);

/**
	@brief Read 4 bytes from port 
*/
unsigned int hal_inl_p(unsigned long port);

/**
	@brief Write 1 byte to port
*/
void hal_outb(unsigned char val, unsigned long port);

/**
	@brief Write 1 byte to port
*/
void hal_outb_p(unsigned char val, unsigned long port);

/**
	@brief Write 2 bytes to port
*/
void hal_outw(unsigned short val, unsigned long port);

/**
	@brief Write 4 bytes to port
*/
void hal_outl(unsigned int val, unsigned long port);

/**
	@brief Write 4 bytes to port
*/
void hal_outl_p(unsigned int val, unsigned long port);

/**
	@brief Read the memory map io
*/
u32 hal_readl(const volatile void *addr);

/**
	@brief Read the memory map io
*/
unsigned char  hal_readb(const volatile void *addr);

/**
	@brief Read the memory map io
*/
void hal_writel(u32 val, volatile void *addr);

#endif

/** @} */
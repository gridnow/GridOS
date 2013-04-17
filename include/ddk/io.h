/**
*  @defgroup HalIoInstructions
*  @ingroup DDK
*
*  ����������IO�����ӿڣ�IOָ���ڲ�ͬƽ̨���ǲ�һ���ģ���������������Σ�ͬʱҲ������һЩ��ͳ����ϰ���Ա�д��inb/outb�����ƵĽӿ�
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
u8 hal_readb(const volatile void *addr);

/**
	@brief Read the memory map io
 */
u16 hal_readw(const volatile void *addr);

/**
	@brief Read the memory map io
*/
u32 hal_readl(const volatile void *addr);

/**
	@brief Read the memory map io
*/
void hal_writeb(u8 val, volatile void *addr);

/**
	@brief Read the memory map io
 */
void hal_writew(u16 val, volatile void *addr);

/**
	@brief Read the memory map io
 */
void hal_writel(u32 val, volatile void *addr);
#endif

/** @} */
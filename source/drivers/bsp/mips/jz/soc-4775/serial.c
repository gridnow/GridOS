
#include <ddk/types.h>
#include <console.h>
#include "jz4770.h"

#define DIV_BEST 13
#define UMR_BEST 16
#define UACR_BEST 0

/* We use the defined port */
#undef UART_BASE 
#define UART_BASE UART2_BASE

static unsigned short quot1[3] = {0}; /* quot[0]:baud_div, quot[1]:umr, quot[2]:uacr */
/* Calculate baud_div, umr and uacr for any frequency of uart */
static unsigned short * get_divisor(void)
{
#ifdef UART_DIV
	REG_CPM_CPCCR |= CPM_CPCCR_ECS;
#else
	REG_CPM_CPCCR &= ~CPM_CPCCR_ECS;
#endif

	quot1[0] = DIV_BEST;
	quot1[1] = UMR_BEST;
	quot1[2] = UACR_BEST;

	return quot1;    
}

void serial_setbrg (void)
{
	volatile u8 *uart_lcr = (volatile u8 *)(UART_BASE + OFF_LCR);
	volatile u8 *uart_dlhr = (volatile u8 *)(UART_BASE + OFF_DLHR);
	volatile u8 *uart_dllr = (volatile u8 *)(UART_BASE + OFF_DLLR);
	volatile u8 *uart_umr = (volatile u8 *)(UART_BASE + OFF_UMR);
	volatile u8 *uart_uacr = (volatile u8 *)(UART_BASE + OFF_UACR);
	u16 baud_div, tmp;

	get_divisor();

#ifndef CONFIG_JZ4730    
	*uart_umr = quot1[1];
	*uart_uacr = quot1[2];
#endif    
	baud_div = quot1[0];

	tmp = *uart_lcr;
	tmp |= UART_LCR_DLAB;
	*uart_lcr = tmp;

	*uart_dlhr = (baud_div >> 8) & 0xff;
	*uart_dllr = baud_div & 0xff;

	tmp &= ~UART_LCR_DLAB;
	*uart_lcr = tmp;
}

int plat_setup_serial (void)
{
	volatile u8 *uart_fcr = (volatile u8 *)(UART_BASE + OFF_FCR);
	volatile u8 *uart_lcr = (volatile u8 *)(UART_BASE + OFF_LCR);
	volatile u8 *uart_ier = (volatile u8 *)(UART_BASE + OFF_IER);
	volatile u8 *uart_sircr = (volatile u8 *)(UART_BASE + OFF_SIRCR);

	/* Disable port interrupts while changing hardware */
	*uart_ier = 0;

	/* Disable UART unit function */
	*uart_fcr = ~UART_FCR_UUE;

	/* Set both receiver and transmitter in UART mode (not SIR) */
	*uart_sircr = ~(SIRCR_RSIRE | SIRCR_TSIRE);

	/* Set databits, stopbits and parity. (8-bit data, 1 stopbit, no parity) */
	*uart_lcr = UART_LCR_WLEN_8 | UART_LCR_STOP_1;

	/* Set baud rate */
	serial_setbrg();

	/* Enable UART unit, enable and clear FIFO */
	*uart_fcr = UART_FCR_UUE | UART_FCR_FE | UART_FCR_TFLS | UART_FCR_RFLS;

	return 0;
}

void serial_putc (const char c)
{
	int timeout = 10000;
	volatile u8 *uart_lsr = (volatile u8 *)(UART_BASE + OFF_LSR);
	volatile u8 *uart_tdr = (volatile u8 *)(UART_BASE + OFF_TDR);

	if (c == '\n') serial_putc ('\r');

	/* Wait for fifo to shift out some bytes */
	while (((uart_lsr[0] & (UART_LSR_TDRQ | UART_LSR_TEMT)) != (UART_LSR_TDRQ | UART_LSR_TEMT)) &&
		timeout--) ;

	*uart_tdr = (u8)c;
}

void serial_puts (const char *s)
{
	//	serial_put_hex((unsigned int)s);
	while (*s) {
		serial_putc (*s++);
	}
	//	serial_put_hex((unsigned int)s);
}

static int write_string(char *buffer, int size)
{
	serial_puts(buffer);
	return size;
}

struct hal_console_ops serial_console_ops = {
	.write = write_string,
};

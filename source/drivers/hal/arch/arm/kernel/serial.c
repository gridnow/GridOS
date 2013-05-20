
#define CONFIG_SERIAL1
typedef enum {
	S3C64XX_UART0,
	S3C64XX_UART1,
	S3C64XX_UART2,
	S3C64XX_UART3,
} S3C64XX_UARTS_NR;

typedef volatile unsigned char		S3C64XX_REG8;
typedef volatile unsigned short		S3C64XX_REG16;
typedef volatile unsigned int		S3C64XX_REG32;

/* UART (see manual chapter 11) */
typedef struct {
	S3C64XX_REG32	ULCON;
	S3C64XX_REG32	UCON;
	S3C64XX_REG32	UFCON;
	S3C64XX_REG32	UMCON;
	S3C64XX_REG32	UTRSTAT;
	S3C64XX_REG32	UERSTAT;
	S3C64XX_REG32	UFSTAT;
	S3C64XX_REG32	UMSTAT;
#ifdef __BIG_ENDIAN
	S3C64XX_REG8	res1[3];
	S3C64XX_REG8	UTXH;
	S3C64XX_REG8	res2[3];
	S3C64XX_REG8	URXH;
#else /* Little Endian */
	S3C64XX_REG8	UTXH;
	S3C64XX_REG8	res1[3];
	S3C64XX_REG8	URXH;
	S3C64XX_REG8	res2[3];
#endif
	S3C64XX_REG32	UBRDIV;
} /*__attribute__((__packed__))*/ S3C64XX_UART;

/*
* UART
*/
#define ELFIN_UART_BASE		0x7F005000

#define ELFIN_UART0_OFFSET	0x0000
#define ELFIN_UART1_OFFSET	0x0400
#define ELFIN_UART2_OFFSET	0x0800
#define ELFIN_UART3_OFFSET	0x0c00

#ifdef CONFIG_SERIAL1
#define ELFIN_UART_CONSOLE_BASE (ELFIN_UART_BASE + ELFIN_UART0_OFFSET)
#elif defined(CONFIG_SERIAL2)
#define ELFIN_UART_CONSOLE_BASE (ELFIN_UART_BASE + ELFIN_UART1_OFFSET)
#elif defined(CONFIG_SERIAL3)
#define ELFIN_UART_CONSOLE_BASE (ELFIN_UART_BASE + ELFIN_UART2_OFFSET)
#elif defined(CONFIG_SERIAL4)
#define ELFIN_UART_CONSOLE_BASE (ELFIN_UART_BASE + ELFIN_UART3_OFFSET)
#else
#define ELFIN_UART_CONSOLE_BASE (ELFIN_UART_BASE + ELFIN_UART0_OFFSET)
#endif

#define ULCON_OFFSET		0x00
#define UCON_OFFSET		0x04
#define UFCON_OFFSET		0x08
#define UMCON_OFFSET		0x0C
#define UTRSTAT_OFFSET		0x10
#define UERSTAT_OFFSET		0x14
#define UFSTAT_OFFSET		0x18
#define UMSTAT_OFFSET		0x1C
#define UTXH_OFFSET		0x20
#define URXH_OFFSET		0x24
#define UBRDIV_OFFSET		0x28
#define UDIVSLOT_OFFSET		0x2C
#define UINTP_OFFSET		0x30
#define UINTSP_OFFSET		0x34
#define UINTM_OFFSET		0x38

#define UTRSTAT_TX_EMPTY	BIT2
#define UTRSTAT_RX_READY	BIT0
#define UART_ERR_MASK		0xF


static inline S3C64XX_UART * S3C64XX_GetBase_UART(S3C64XX_UARTS_NR nr)
{
	//	return (S3C64XX_UART *)(ELFIN_UART_BASE + (nr * 0x4000));
	return (S3C64XX_UART *)(ELFIN_UART_BASE + (nr*0x400));
}


#ifdef CONFIG_SERIAL1
#define UART_NR	S3C64XX_UART0

#elif defined(CONFIG_SERIAL2)
#define UART_NR	S3C64XX_UART1

#elif defined(CONFIG_SERIAL3)
#define UART_NR	S3C64XX_UART2

#elif defined(CONFIG_SERIAL4)
#define UART_NR	S3C64XX_UART3

#else
#error "Bad: you didn't configure serial ..."
#endif

void serial_setbrg(void)
{
	//DECLARE_GLOBAL_DATA_PTR;

	int i;
	for (i = 0; i < 100; i++);
}

/*
* Initialise the serial port with the given baudrate. The settings
* are always 8 data bits, no parity, 1 stop bit, no start bits.
*
*/
int serial_init(void)
{
	serial_setbrg();

	return (0);
}

/*
* Read a single byte from the serial port. Returns 1 on success, 0
* otherwise. When the function is succesfull, the character read is
* written into its argument c.
*/
int serial_getc(void)
{
	S3C64XX_UART *const uart = S3C64XX_GetBase_UART(UART_NR);

	/* wait for character to arrive */
	while (!(uart->UTRSTAT & 0x1));

	return uart->URXH & 0xff;
}

#ifdef CONFIG_HWFLOW
static int hwflow = 0;		/* turned off by default */
int hwflow_onoff(int on)
{
	switch (on) {
	case 0:
	default:
		break;		/* return current */
	case 1:
		hwflow = 1;	/* turn on */
		break;
	case -1:
		hwflow = 0;	/* turn off */
		break;
	}
	return hwflow;
}
#endif

#ifdef CONFIG_MODEM_SUPPORT
static int be_quiet = 0;
void disable_putc(void)
{
	be_quiet = 1;
}

void enable_putc(void)
{
	be_quiet = 0;
}
#endif


/*
* Output a single byte to the serial port.
*/
void serial_putc(const char c)
{
	S3C64XX_UART *const uart = S3C64XX_GetBase_UART(UART_NR);

#ifdef CONFIG_MODEM_SUPPORT
	if (be_quiet)
		return;
#endif

	/* wait for room in the tx FIFO */
	while (!(uart->UTRSTAT & 0x2));

#ifdef CONFIG_HWFLOW
	/* Wait for CTS up */
	while (hwflow && !(uart->UMSTAT & 0x1));
#endif

	uart->UTXH = c;

	/* If \n, also do \r */
	if (c == '\n')
		serial_putc('\r');
}

/*
* Test whether a character is in the RX buffer
*/
int serial_tstc(void)
{
	S3C64XX_UART *const uart = S3C64XX_GetBase_UART(UART_NR);

	return uart->UTRSTAT & 0x1;
}

void serial_puts(const char *s)
{
	while (*s) {
		serial_putc(*s++);
	}
}

static int write_string(char *buffer, int size)
{
	serial_puts(buffer);
}

#include <console.h>
struct hal_console_ops serial_console_ops = {
	.write = write_string,
};
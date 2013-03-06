/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   Serial operation for x86
*/

#include <types.h>
#include <console.h>

#include <asm/io.h>
#include <asm/serial.h>

enum serial_register_offsets {
	SERIAL_TRANSMIT_BUFFER		= 0,
	SERIAL_RECEIVE_BUFFER		= 0,
	SERIAL_DIVISOR_LATCH_LOW	= 0,
	SERIAL_DIVISOR_LATCH_HIGH	= 1,
	SERIAL_FIFO_CONTROL			= 2,
	SERIAL_LINE_CONTROL			= 3,
	SERIAL_MODEM_CONTROL		= 4,
	SERIAL_LINE_STATUS			= 5,
	SERIAL_MODEM_STATUS			= 6,
};

#define kSerialBaudRate  115200
#define sSerialBasePort  0x3f8

static void serial_putc(char c)
{
	// wait until the transmitter empty bit is set
	while ((inb(sSerialBasePort + SERIAL_LINE_STATUS) & 0x20) == 0);

	outb(c, sSerialBasePort + SERIAL_TRANSMIT_BUFFER);
}

 void serial_puts(const char* string, int size)
{
	while (size-- != 0 && *string != 0) {
		char c = string[0];

		if (c == '\n') {
			serial_putc('\r');
			serial_putc('\n');
		} else 
			serial_putc(c);

		string++;
	}
}

static int write_string(char *buffer, int size)
{
	serial_puts(buffer, size);
}

void __init x86_serial_init(void)
{
	u16 divisor = 115200 / kSerialBaudRate;

	outb(0x80, sSerialBasePort + SERIAL_LINE_CONTROL);
	// set divisor latch access bit
	outb(divisor & 0xf, sSerialBasePort + SERIAL_DIVISOR_LATCH_LOW);
	outb(divisor >> 8, sSerialBasePort + SERIAL_DIVISOR_LATCH_HIGH);
	outb(3, sSerialBasePort + SERIAL_LINE_CONTROL);
	// 8N1
}

struct hal_console_ops serial_console_ops = {
	.write = write_string,
};

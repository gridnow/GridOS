#include <compiler.h>

#define CONFIG_IO_DELAY_TYPE_0X80 0x80
#define CONFIG_IO_DELAY_TYPE_0XED 0xed
#define CONFIG_DEFAULT_IO_DELAY_TYPE CONFIG_IO_DELAY_TYPE_0X80
int io_delay_type __read_mostly = CONFIG_DEFAULT_IO_DELAY_TYPE;

static int __initdata io_delay_override;

/*
* Paravirt wants native_io_delay to be a constant.
*/
void native_io_delay(void)
{
	switch (io_delay_type) {
	default:
	case CONFIG_IO_DELAY_TYPE_0X80:
		asm volatile ("outb %al, $0x80");
		break;
	case CONFIG_IO_DELAY_TYPE_0XED:
		asm volatile ("outb %al, $0xed");
		break;
#if 0
	case CONFIG_IO_DELAY_TYPE_UDELAY:
		/*
		* 2 usecs is an upper-bound for the outb delay but
		* note that udelay doesn't have the bus-level
		* side-effects that outb does, nor does udelay() have
		* precise timings during very early bootup (the delays
		* are shorter until calibrated):
		*/
		udelay(2);
	case CONFIG_IO_DELAY_TYPE_NONE:
		break;
#endif
	}
}


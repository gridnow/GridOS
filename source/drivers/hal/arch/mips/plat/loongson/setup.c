#include <screen.h>
#include <types.h>
#include <hal_string.h>
#include <hal_debug.h>
#include <hal_irqflags.h>
#include <cmdline.h>
 
#include <asm/plat/setup.h>
#include <ASM/MIPSREGs.h>
#include <asm/smp.h>
#include <asm/io.h>

#include <loongson.h>

/* stuff for 3a specific */
#include "3a/smp.h"

static void wbflush_loongson(void)
{
	asm(".set\tpush\n\t"
		".set\tnoreorder\n\t"
		".set mips3\n\t"
		"sync\n\t"
		"nop\n\t"
		".set\tpop\n\t"
		".set mips0\n\t");
}

void (*__wbflush)(void) = wbflush_loongson;


void plat_setup_screen(struct video_screen_info *main_screen)
{
	char *v;
	
	/* The basic info ,should get from the config or the command line */
	main_screen->fb_base	= (unsigned char*)0xffffffffb0000000;
	
	/* machine specific irq init */
	if (hal_cmdline_get_string_value("PLAT", &v) == true)
	{
		if (!strcmp(v, "2f"))
		{
			main_screen->bpp		= 8;
			main_screen->width		= 640;
			main_screen->height		= 400;
		}
		else if (!strcmp(v, "2fpc"))
		{
			main_screen->bpp		= 16;
			main_screen->width		= 800;
			main_screen->height		= 600;
		}
		else if (!strcmp(v, "3aserver"))
		{
			main_screen->bpp		= 16;
			main_screen->width		= 800;
			main_screen->height		= 600;
		}
		else if(!strcmp(v, "3a"))
		{
			main_screen->bpp		= 16;
			main_screen->width		= 800;
			main_screen->height		= 600;
			main_screen->fb_base	= (unsigned char*)0x9000000040000000;

		}
		else
		{
			hal_panic("平台参数给定不对，初始化不了FRAME BUFFER");
		}
	}
	else
	{
		main_screen->bpp		= 16;
		main_screen->width		= 800;
		main_screen->height		= 600;
	}
	
	memset(main_screen->fb_base, 0, main_screen->width * main_screen->height * main_screen->bpp / 8);
}

void plat_setup_ioport()
{
	char *v;

	if (hal_cmdline_get_string_value("PLAT", &v) == true)
	{
		if (!strncmp(v, "2f", 2))
			set_io_port_base((unsigned long)CKSEG1ADDR(LOONGSON_PCIIO_BASE)); /* LOONGSON_PCIIO_SIZE */
		else if (!strncmp(v, "3a", 2))
			set_io_port_base((unsigned long)0x9000000000000000UL + 0xefdfc000000);
	}
}
/**
	@brief the plat is setting up the SMP, called at the upper SMP starting up.
*/
void plat_init_smp()
{
#ifdef CONFIG_SMP
	register_smp_ops(&loongson3_smp_ops);
#endif
}

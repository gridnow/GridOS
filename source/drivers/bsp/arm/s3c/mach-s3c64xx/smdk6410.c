/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   Ben Dooks <ben@simtec.co.uk>
 *   ARM S3C6410 Platform initializing
 *
 */
#include <stddef.h>

#include <ddk/compiler.h>
#include <ddk/types.h>

#include <mach/arch.h>
#include <mach/map.h>

#include <mach-types.h>

#include "common.h"

static struct map_desc smdk6410_iodesc[] = {};

static void __init smdk6410_map_io(void)
{
	u32 tmp;
	
	s3c64xx_init_io(smdk6410_iodesc, ARRAY_SIZE(smdk6410_iodesc));
#if 0
	s3c24xx_init_clocks(12000000);
	s3c24xx_init_uarts(smdk6410_uartcfgs, ARRAY_SIZE(smdk6410_uartcfgs));
	
	/* set the LCD type */
	
	tmp = __raw_readl(S3C64XX_SPCON);
	tmp &= ~S3C64XX_SPCON_LCD_SEL_MASK;
	tmp |= S3C64XX_SPCON_LCD_SEL_RGB;
	__raw_writel(tmp, S3C64XX_SPCON);
	
	/* remove the lcd bypass */
	tmp = __raw_readl(S3C64XX_MODEM_MIFPCON);
	tmp &= ~MIFPCON_LCD_BYPASS;
	__raw_writel(tmp, S3C64XX_MODEM_MIFPCON);
#endif
}

MACHINE_START(SMDK6410, "SMDK6410")
	.atag_offset	= 0x100,

	.init_irq		= 0,
	.handle_irq		= 0,
	.map_io			= 0,
	.init_machine	= 0,
	.init_late		= 0,
	.timer			= 0,
	.restart		= 0,
MACHINE_END

struct machine_desc *get_s3c6410_machine_desc()
{
	return (struct machine_desc *)&__mach_desc_SMDK6410;
}

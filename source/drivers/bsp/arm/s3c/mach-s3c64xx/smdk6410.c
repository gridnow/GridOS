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
#include <ddk/compatible.h>

#include <plat/map-base.h>
#include <mach/arch.h>
#include <mach-types.h>
#include "mach/map.h"
#include "mach/map_s3c.h"

#include "common.h"

static struct map_desc smdk6410_iodesc[] = {
	{
		.virtual	= (unsigned long)S3C_VA_LCD,
		.pfn		= __phys_to_pfn(S3C64XX_PA_FB),
		.length		= SZ_16K,
		.type		= MT_DEVICE,
	},
};

static void __init smdk6410_map_io(void)
{
	s3c64xx_init_io(smdk6410_iodesc, ARRAY_SIZE(smdk6410_iodesc));
	//s3cfb_init_lcd();
}

MACHINE_START(SMDK6410, "SMDK6410")
	.atag_offset	= 0x100,

	.init_irq		= 0,
	.handle_irq		= 0,
	.map_io			= smdk6410_map_io,
	.init_machine	= 0,
	.init_late		= 0,
	.timer			= 0,
	.restart		= 0,
MACHINE_END

struct machine_desc *get_s3c6410_machine_desc()
{
	return (struct machine_desc *)&__mach_desc_SMDK6410;
}

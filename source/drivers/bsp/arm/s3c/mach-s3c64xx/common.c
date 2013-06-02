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

#include <plat/map-base.h>
#include "include/mach/map.h"

/* see notes on uart map in arch/arm/mach-s3c64xx/include/mach/debug-macro.S */
#define UART_OFFS (S3C_PA_UART & 0xfffff)

static struct map_desc s3c_iodesc[] __initdata = {
	{
		.virtual	= (unsigned long)S3C_VA_SYS,
		.pfn		= __phys_to_pfn(S3C64XX_PA_SYSCON),
		.length		= SZ_4K,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)S3C_VA_MEM,
		.pfn		= __phys_to_pfn(S3C64XX_PA_SROM),
		.length		= SZ_4K,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)(S3C_VA_UART + UART_OFFS),
		.pfn		= __phys_to_pfn(S3C_PA_UART),
		.length		= SZ_4K,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)VA_VIC0,
		.pfn		= __phys_to_pfn(S3C64XX_PA_VIC0),
		.length		= SZ_16K,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)VA_VIC1,
		.pfn		= __phys_to_pfn(S3C64XX_PA_VIC1),
		.length		= SZ_16K,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)S3C_VA_TIMER,
		.pfn		= __phys_to_pfn(S3C_PA_TIMER),
		.length		= SZ_16K,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)S3C64XX_VA_GPIO,
		.pfn		= __phys_to_pfn(S3C64XX_PA_GPIO),
		.length		= SZ_4K,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)S3C64XX_VA_MODEM,
		.pfn		= __phys_to_pfn(S3C64XX_PA_MODEM),
		.length		= SZ_4K,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)S3C_VA_WATCHDOG,
		.pfn		= __phys_to_pfn(S3C64XX_PA_WATCHDOG),
		.length		= SZ_4K,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)S3C_VA_USB_HSPHY,
		.pfn		= __phys_to_pfn(S3C64XX_PA_USB_HSPHY),
		.length		= SZ_1K,
		.type		= MT_DEVICE,
	},
};

void __init s3c64xx_init_io(struct map_desc *mach_desc, int size)
{
	/* Initialise the io descriptors we need for initialisation */
	arm_bsp_create_map(s3c_iodesc, ARRAY_SIZE(s3c_iodesc));
	arm_bsp_create_map(mach_desc, size);
	
#if 0
	/* detect cpu id */
	s3c64xx_init_cpu();
	
	s3c_init_cpu(samsung_cpu_id, cpu_ids, ARRAY_SIZE(cpu_ids));
#endif
}

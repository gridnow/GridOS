#include <types.h>
#include <asm/x86_init.h>


void __cpuinit x86_init_noop(void) { }
void __init x86_init_uint_noop(unsigned int unused) { }
int __init iommu_init_noop(void) { return 0; }
void iommu_shutdown_noop(void) { }

/*
 * The platform setup functions are preset with the default functions
 * for standard PC hardware.
 */
struct x86_init_ops x86_init __initdata = {

	
};

static void default_nmi_init(void) { };
static int default_i8042_detect(void) { return 1; };

struct x86_platform_ops x86_platform = {
	.iommu_shutdown			= iommu_shutdown_noop,
	.nmi_init			= default_nmi_init,
	.i8042_detect			= default_i8042_detect,
};


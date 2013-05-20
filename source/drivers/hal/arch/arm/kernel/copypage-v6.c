#include <arch/page.h>

struct cpu_user_fns v6_user_fns __initdata = {
	.cpu_clear_user_highpage = 0,
	.cpu_copy_user_highpage	= 0,
};

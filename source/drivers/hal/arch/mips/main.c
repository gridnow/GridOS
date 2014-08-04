#include <string.h>

#include <arch/arch.h>
#include <screen.h>
#include <limits.h>

#include <asm/irq.h>
#include <asm/smp.h>
#include <asm/cpu-info.h>

/* per cpuinfo */
struct cpuinfo_mips cpu_data[NR_CPUS];

/* pointer from fareware */
unsigned long fw_arg0, fw_arg1, fw_arg2, fw_arg3;
/* IO BASE ,refered in asm/io.h */
const unsigned long mips_io_port_base = -1;
void hal_arch_init(int step)
{
	switch(step)
	{
	case HAL_ARCH_INIT_PHASE_EARLY:
		//plat_setup_serial();
//		video_init_screen();
//		plat_setup_ioport();
		/* Init the boot cpu, the secondary cpu should also probe it self when started */
		cpu_probe();
		arch_trap_init();
		mips_setup_traps();
		break;
	case HAL_ARCH_INIT_PHASE_MIDDLE:
		arch_init_irq();
		arch_init_smp();
		break;
	case HAL_ARCH_INIT_PHASE_LATE:
		smp_boot_aps();
		break;
	}
}

void arch_video_init_screen(struct video_screen_info *main_screen)
{
	//plat_setup_screen(main_screen);
}

void arch_get_fw_arg(unsigned long *argc, unsigned long *argv)
{
	if (argc) *argc = (long)fw_arg0;
	if (argv) *argv = (long)fw_arg1;
}

int arch_get_fw_arg_pointer_size()
{
	return 4;
}

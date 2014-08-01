#ifndef ARCH_MIPS_PLAT_SETUP_H
#define ARCH_MIPS_PLAT_SETUP_H

struct video_screen_info;
void plat_setup_screen(struct video_screen_info *screen);

/**
	@brief Setup the irq
*/
void plat_init_irq();

/**
	@brief setup the smp 
*/
void plat_init_smp();

/**
	@brief Setup the io port base
*/
void plat_setup_ioport();

#endif

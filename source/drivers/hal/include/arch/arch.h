/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL Arch Common description
*/
#ifndef HAL_ARCH_H
#define HAL_ARCH_H
#include <compiler.h>
#include <types.h>

extern void __init __noreturn hal_main();

struct video_screen_info;

/********************************************************************
				平台要提供如下通用接口，以便HAL能初始化
********************************************************************/
/**
	@brief HAL主初始化流程，让平台分阶段做初始化
*/
extern void hal_arch_init(int step);

/**
	@brief 硬件平台名称
*/
extern const xstring hal_arch_get_name();

/**
	@brief 设置cpu基地址
*/
extern void hal_arch_setup_percpu(int cpu, unsigned long base);


extern void hal_arch_video_init_screen(struct video_screen_info *main_screen);

#define HAL_ARCH_INIT_PHASE_EARLY 	0
#define HAL_ARCH_INIT_PHASE_MIDDLE 	1
#define HAL_ARCH_INIT_PHASE_LATE 	2
#endif


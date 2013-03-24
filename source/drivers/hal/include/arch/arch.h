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
void hal_arch_video_init_screen(struct video_screen_info *main_screen);

#define HAL_ARCH_INIT_PHASE_EARLY 	0
#define HAL_ARCH_INIT_PHASE_MIDDLE 	1
#define HAL_ARCH_INIT_PHASE_LATE 	2
#endif


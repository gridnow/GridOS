/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HALģ��������������
*/

#ifndef HAL_LINKAGE_H
#define HAL_LINKAGE_H

#include <asm/linkage.h>

#ifdef __cplusplus
#define CPP_ASMLINKAGE extern "C"
#else
#define CPP_ASMLINKAGE
#endif

#ifndef asmlinkage
#define asmlinkage CPP_ASMLINKAGE
#endif

#ifndef asmregparm
# define asmregparm
#endif

#ifndef __ALIGN
#define __ALIGN		.align 4,0x90
#define __ALIGN_STR	".align 4,0x90"
#endif

#ifdef __ASSEMBLY__
#define ALIGN __ALIGN
#define ALIGN_STR __ALIGN_STR

#ifndef ENTRY
#define ENTRY(name) \
	.globl name; \
	ALIGN; \
name:
#endif

#ifndef END
#define END(name) \
	.size name, .-name
#endif

#ifndef ENDPROC
#define ENDPROC(name) \
	.type name, @function; \
	END(name)
#endif

/* For assembly routines */
#define __HEAD		.section	".head.text","ax"
#define __INIT		.section	".init.text","ax"
#define __FINIT		.previous

#define __INITDATA	.section	".init.data","aw",%progbits
#define __INITRODATA	.section	".init.rodata","a",%progbits
#define __FINITDATA	.previous

#define __CPUINIT        .section	".cpuinit.text", "ax"
#define __CPUINITDATA    .section	".cpuinit.data", "aw"
#define __CPUINITRODATA  .section	".cpuinit.rodata", "a"

#endif /* __ASSEMBLY__ */

#endif /* HAL_LINKAGE_H */
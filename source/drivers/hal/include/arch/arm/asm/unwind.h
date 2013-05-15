
#ifndef __ASM_UNWIND_H
#define __ASM_UNWIND_H

#ifdef CONFIG_ARM_UNWIND
#define UNWIND(code...)		code
#else
#define UNWIND(code...)
#endif

#endif

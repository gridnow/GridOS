#ifndef _ASM_FPU_H
#define _ASM_FPU_H

#include <asm/mipsregs.h>
#include <asm/cpu.h>
#include <asm/cpu-features.h>
#include <asm/hazards.h>
#include <asm/processor.h>


#define __enable_fpu()							\
do {									\
        set_c0_status(ST0_CU1);						\
        enable_fpu_hazard();						\
} while (0)

#endif

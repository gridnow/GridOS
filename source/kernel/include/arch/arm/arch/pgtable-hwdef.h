
#ifndef _ASMARM_PGTABLE_HWDEF_H
#define _ASMARM_PGTABLE_HWDEF_H

#ifdef CONFIG_ARM_LPAE
#include "pgtable-3level-hwdef.h"
#else
#include "pgtable-2level-hwdef.h"
#endif

#endif

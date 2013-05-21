
#ifndef _ASM_PGTABLE_2LEVEL_TYPES_H
#define _ASM_PGTABLE_2LEVEL_TYPES_H

#ifndef __ASSEMBLER__
#include <types.h>
#endif

typedef u32 pteval_t;
typedef u32 pmdval_t;

typedef pteval_t pgprot_t;

#define pgprot_val(x)   (x)
#define __pgprot(x)     (x)


#endif	/* _ASM_PGTABLE_2LEVEL_TYPES_H */

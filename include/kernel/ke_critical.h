/**
   @defgroup critical_section
   @ingroup kernel_api

   内核编程接口 临界区域

   @{
*/
#ifndef KE_CRITICAL_H
#define KE_CRITICAL_H

#include "ke_complete.h"
struct ke_critical_section
{
	struct ke_spinlock lock;
	unsigned int deal_count;
	struct ke_completion complete;
};

DLLEXPORT void ke_critical_init(struct ke_critical_section * cs);
DLLEXPORT void ke_critical_enter(struct ke_critical_section * cs);
DLLEXPORT void ke_critical_leave(struct ke_critical_section * cs);

#endif

/** @} */

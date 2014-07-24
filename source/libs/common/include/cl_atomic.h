/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   wuxin
 *   atomic
 */

#ifndef _CL_ATOMIC_H
#define _CL_ATOMIC_H

#include <kernel/ke_atomic.h>

/* 类型就复用内核定义的 */
void cl_atomic_inc(struct ke_atomic *atomic);
void cl_atomic_dec(struct ke_atomic *atomic);
#endif
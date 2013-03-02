/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
* 
*   HAL SMP
*/

#ifndef HAL_SMP_H
#define HAL_SMP_H

extern int raw_smp_processor_id();
#define smp_processor_id() raw_smp_processor_id()

#endif

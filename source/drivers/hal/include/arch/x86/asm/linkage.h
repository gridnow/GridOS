/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   X86 ¡¥Ω”Ãÿ–‘√Ë ˆ
*/

#ifndef X86_LINKAGE_H
#define X86_LINKAGE_H

#undef notrace
#define notrace __attribute__((no_instrument_function))

#ifdef CONFIG_X86_32
#define asmlinkage CPP_ASMLINKAGE __attribute__((regparm(0)))
#define asmregparm __attribute__((regparm(3)))

#endif

#endif /* X86 */

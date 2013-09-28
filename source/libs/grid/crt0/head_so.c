
#include <types.h>

extern int dll_main(int argc, char ** argv);

#if defined(__i386__) || defined(__arm__)
void _start(unsigned long para)
#elif defined(__mips__)
void __start(unsigned long para)
#endif
{
	dll_main(0, 0);
}
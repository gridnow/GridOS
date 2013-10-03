
#include <types.h>

extern int dll_main(int argc, char ** argv);

void _start(unsigned long para)
{
	dll_main(0, 0);
}
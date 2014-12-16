
#include "spin.c"
#include "create.c"
#include "ihash.c"
#include "pthread_key.c"
#include "pthread_specific.c"
#include "pthread_once.c"

void dll_main(int x, int y)
{
	pthread_locks_init();
}
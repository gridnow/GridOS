#include "cmd.h"

static int _null(int argc, char * argv[])
{
	 
} 

struct cmd CMD_ENTRY cmd__null = {
	.name = 0,
	.desc = 0,
	.help = 0,
	.func = 0,
};

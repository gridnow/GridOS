/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

/**
	����̨����
	Wuxin (82828068@qq.com)
 */

#include <stdio.h>
#include "cmd.h"

static int rm(int argc, char * argv[])
{
	TODO("");
	return ENOSYS;
}

struct cmd CMD_ENTRY cmd_rm = {
	.name = "rm",
	.desc = "ɾ���ļ���Ŀ¼",
	.help = "TODO",
	.func = rm,
};

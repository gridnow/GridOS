/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

/**
 
	控制台命令
	Wuxin (82828068@qq.com)
 */
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "cmd.h"

static int cd(int argc, char * argv[])
{
	int r;
	if (argc == 1)
		return EINVAL;
	
	r = chdir(argv[1]);
	printf("ret is %d\n", r);
	return r;
} 

struct cmd CMD_ENTRY cmd_cd = {
	.name = "cd",
	.desc = "切换工作目录",
	.help = "无帮助信息\n",		
	.func = cd,
};

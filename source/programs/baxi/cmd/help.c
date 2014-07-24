/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

/**
	控制台命令
	Wuxin (82828068@qq.com)
 */

#include <stdio.h>
#include "cmd.h"

#define USAGE "1，获取所有内置命令的简短帮助请输入help；2，获取特定命令的帮助请输入help xxx(如help cd)。\n"

static int help(int argc, char * argv[])
{
	struct cmd * p;

	/* 没有参数，则列出所有命令 */
	if (argc == 1)
	{		
		p = &cmd__head;
		printf("BAXI 内置命令有：\n");	
		while ((++p)->name != NULL)
		{
			printf("%s\t %s.\n", p->name, p->desc);		
		}
	}

	/* 有一个参数，那么可能是：help ls 之类的，仅仅显示特定命令的帮助 */
	else if (argc == 2)
	{
		p = command_find(argv[1], false);
		if (p)
		{
			printf("命令 %s 的用法是:\n%s.", argv[1], p->help);
		}
		else
		{
			printf("需要帮助的命令 %s 没有找到。\n", argv[1]);			
		}
	}
	else
	{
		printf("本命令使用方式是：%s。\n", USAGE);
	}

	return CMD_RETURN_OK;
}

struct cmd CMD_ENTRY cmd_help = {
	.name = "help",
	.desc = "baxi 帮助",
	.help = USAGE,
	.func = help,
};

/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

/**
	控制台命令
	Wuxin (82828068@qq.com)
 */

#include <stdio.h>
#include <dirent.h>
#include <getopt.h>

#include "cmd.h"

static int show_normal()
{
	DIR  	*input;
	struct 	dirent * ptr;
	
	input = opendir("/os/");
	if (!input)
		return ENOENT;
	while ((ptr = readdir(input)) != NULL)
	{
		printf(ptr->d_name);
		printf(" ");
	}
	printf("\n");
	closedir(input);

	return CMD_RETURN_OK;
}

static int ls(int argc, char * argv[])
{
	show_normal();
	return CMD_RETURN_OK;
}

struct cmd CMD_ENTRY cmd_ls = {
	.name = "ls",
	.desc = "列出当前目录下的文件信息",
	.help = "-l 详细信息\n"
			"-a 列出隐藏的文件\n",
	.func = ls,
};

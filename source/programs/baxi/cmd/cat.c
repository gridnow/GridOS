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

static int cat(int argc, char * argv[])
{
	FILE *file;
	char str[5] = {0};
	int ret;
	
	/* 目前只支持一个文件的CAT */
	if (argc != 2)
		goto err;
	
	file = fopen(argv[1], "r");
	if (!file)
	{
		printf("%s 打开失败。\n", argv[1]);
		goto err;
	}
	while (((ret = fread(str, 2, 1, file))) != 0)
	{
		printf("%s", str);
	}
	fclose(file);
	return CMD_RETURN_OK;

err:
	return EINVAL;
} 

struct cmd CMD_ENTRY cmd_cat = {
	.name = "cat",
	.desc = "显示文件内容，或者追加一个文件到另外一个文件",
	.help = "cat的帮助信息有待完善\n",			
	.func = cat,
};

/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

/**
	控制台命令
	Wuxin (82828068@qq.com)
 */

#include <stdio.h>
//#include <dirent.h>
#include <getopt.h>

#include "cmd.h"
#if 0
static int show_normal()
{
	DIR  	*input;
	struct 	dirent * ptr;
	
	input = opendir(".");
	while ((ptr = readdir(input)) != NULL)
	{
		//if(strcmp(".",ptr->d_name) == 0 || strcmp("..",ptr->d_name) == 0)
		//	continue;
		printf(ptr->d_name);
		printf(" ");
	}
	printf("\n");
	closedir(input);
}
#endif
static int ls(int argc, char * argv[])
{
#if 0
	int opt;
	struct option long_help[]=
	{
		{"all",			0,	NULL,'a'},
		{"directory",		0,	NULL,'d'},
		{"help",		0,	NULL,'h'},
		//{"human-readable",	0,	NULL,'h'},
		{"inode",		0,	NULL,'i'},
		{"reverse",		0,	NULL,'r'},
		{"recursive",		0,	NULL,'R'},
		{0,0,0,0},			
	};

	/* Normal mode */
	if (argc == 1)
		show_normal();
	else
	{
		while ((opt = getopt_long(argc, argv, "adhirR", long_help, NULL)) != -1)
		{
			switch(opt)
			{
			default:
				break;
			}
		}
	}
#endif
	TODO("");
	return ENOSYS;
} 

struct cmd CMD_ENTRY cmd_ls = {
	.name = "ls",
	.desc = "列出当前目录下的文件信息",
	.help = "-l 详细信息\n"
			"-a 列出隐藏的文件\n",
	.func = ls,
};

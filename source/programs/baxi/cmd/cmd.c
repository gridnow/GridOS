/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

/**
 
	控制台命令
	Wuxin (82828068@qq.com)
 */
#include <types.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <ystd.h>

#include "cmd.h" 

#define MAX_ARGV 16

static bool handle_exe(char * cmdline)
{
	y_handle process;
	unsigned long ret = 1234;

	/* Cmdline 一般保存的是用户的直接输入，启动一个程序时没有绝对路径，因此加上 */
	char str[128];

	//TODO: 获取合适的路径
	memset(str, 0, sizeof(str));
	strcat(str, "0:/os/i386/");
	strcat(str, cmdline);
	if ((process = y_process_create(cmdline, str)) == Y_INVALID_HANDLE)
	{
		return false;
	}

	y_process_wait_exit(process, &ret);
	
	return true;
}

static int dummy(int argc, char * argv[])
{
	return 0;
}

struct cmd CMD_ENTRY cmd__head = {
	.name = NULL,
	.desc = NULL,
	.func = dummy,
};

struct cmd *command_find(char * name, int try_binary)
{
	int name_length = strlen(name);
	struct cmd * p = &cmd__head;

	/* Loop the list */
	while ((++p)->name != NULL)
	{
		int raw_len = strlen(p->name);		

		/* printf("匹配命令:%s.\n", p->name); */
		if (!strncmp(p->name, name, raw_len)/* 名字部分要满足 */ && 
			p->name[raw_len] == 0/* DST 确定是找完了的 */)
		{
			/* 命令行的长度要比命令长，那么有必要比对后面是否是空格或者0结束符，否则命令行中的命令字符串和名命名有可能是前部分一致 */
			if (name_length > raw_len)
			{
				if (name[raw_len] == ' ' || 
					name[raw_len] == '	' ||
					name[raw_len] == 0)
					return p;
			}
			else
				return p;
		}
	} 

	/* May be exe file */
	if (try_binary == true && handle_exe(name))
		return &cmd__head;
	return NULL;
}

int command_exec(void * cmd, char * cmdline)
{
	char *argv[MAX_ARGV];
	struct cmd *p = cmd;
	int r, argc;

	/* Handle argv argc */
	r = crt0_split_cmdline(cmdline, strlen(cmdline), &argc, MAX_ARGV, argv);
	if (r) return r;

	/* Run */
	r = p->func(argc, argv);
	return r;
}

void command_show_all()
{
	struct cmd *p = &cmd__head;
	
	while ((++p)->name != NULL)
	{
		printf("命令名：%s \n", p->name);		
	}
}
/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

/**
 
	����̨����
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

	/* Cmdline һ�㱣������û���ֱ�����룬����һ������ʱû�о���·������˼��� */
	char str[128];

	//TODO: ��ȡ���ʵ�·��
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

		/* printf("ƥ������:%s.\n", p->name); */
		if (!strncmp(p->name, name, raw_len)/* ���ֲ���Ҫ���� */ && 
			p->name[raw_len] == 0/* DST ȷ���������˵� */)
		{
			/* �����еĳ���Ҫ���������ô�б�Ҫ�ȶԺ����Ƿ��ǿո����0�������������������е������ַ������������п�����ǰ����һ�� */
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
		printf("��������%s \n", p->name);		
	}
}
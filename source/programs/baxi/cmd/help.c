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

#define USAGE "1����ȡ������������ļ�̰���������help��2����ȡ�ض�����İ���������help xxx(��help cd)��\n"

static int help(int argc, char * argv[])
{
	struct cmd * p;

	/* û�в��������г��������� */
	if (argc == 1)
	{		
		p = &cmd__head;
		printf("BAXI ���������У�\n");	
		while ((++p)->name != NULL)
		{
			printf("%s\t %s.\n", p->name, p->desc);		
		}
	}

	/* ��һ����������ô�����ǣ�help ls ֮��ģ�������ʾ�ض�����İ��� */
	else if (argc == 2)
	{
		p = command_find(argv[1], false);
		if (p)
		{
			printf("���� %s ���÷���:\n%s.", argv[1], p->help);
		}
		else
		{
			printf("��Ҫ���������� %s û���ҵ���\n", argv[1]);			
		}
	}
	else
	{
		printf("������ʹ�÷�ʽ�ǣ�%s��\n", USAGE);
	}

	return CMD_RETURN_OK;
}

struct cmd CMD_ENTRY cmd_help = {
	.name = "help",
	.desc = "baxi ����",
	.help = USAGE,
	.func = help,
};

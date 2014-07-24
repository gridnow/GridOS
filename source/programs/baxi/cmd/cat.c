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

static int cat(int argc, char * argv[])
{
	FILE *file;
	char str[5] = {0};
	int ret;
	
	/* Ŀǰֻ֧��һ���ļ���CAT */
	if (argc != 2)
		goto err;
	
	file = fopen(argv[1], "r");
	if (!file)
	{
		printf("%s ��ʧ�ܡ�\n", argv[1]);
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
	.desc = "��ʾ�ļ����ݣ�����׷��һ���ļ�������һ���ļ�",
	.help = "cat�İ�����Ϣ�д�����\n",			
	.func = cat,
};

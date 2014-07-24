/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

/**
	
	ϵͳ�ϵļ򵥿���̨
	Wuxin (82828068@qq.com)
 */

#include <types.h>
#include <stdio.h>
#include <string.h>
#include "cmd/cmd.h"

#define SHELL_BANNER "Grid����ϵͳ������,Copyright GridOS Team"
#define SHELL_VERSION 1
#define SHELL_KEY_ENTER 0x0d
#define MAX_LINE_SIZE 1024									// һ�������ֽ�
static char line_buffer[MAX_LINE_SIZE];

/**
	@brief ���������϶�ȡ����
	
	�Իس�����CTRL+C �˳�.
	
	@return
		�����˳�ʱ���ĸ���
*/	
static int read_input(char * line_buffer, int max_size)
{
	int cur_pos = 0;
	
	line_buffer[0] = 0;
	do
	{
		/* ��ȡ�ַ� */
		int ch = getch();
		
		/* ���𣿺��� */
		if (ch == 0) 
		{			
			continue;
		}

		/* ��һ�������İ����� ������ж�� */

		/* ���˳����ļ���*/
		if (ch == SHELL_KEY_ENTER)
		{			
			return ch;
		}

		/* �����뵽������ */
		line_buffer[cur_pos++] = ch;
		line_buffer[cur_pos] = 0;
		putchar(ch);

	} while (cur_pos < max_size - 1/*end code */);

	/* ��ʱ�ѽ��ﵽ��󻺳��������µ��ַ�������ô��ʱ�����ٽ����ˣ�������壿*/
	while (1)
	{
		if (getch() == SHELL_KEY_ENTER)
			return SHELL_KEY_ENTER;
		else
		{
			/* ���� */
		}
	}
}

/**
	@brief ִ������
*/
static void execute_cmd(char * line_buffer)
{
	void * cmd;

	/* �����˻س��� */
	if (!strlen(line_buffer)) return;

// 	printf("ִ�����%s(%d���ֽ�)��\n", line_buffer, max_size);
	cmd = command_find(line_buffer, true);
	if (!cmd)
	{
		printf("û���ҵ�����������\n");
		return;
	}

	if (command_exec(cmd, line_buffer))
	{
		printf("����ִ��ʧ�ܡ�\n");
	}
}

int main(int argc, char * argv[])
{	
	char current_path[] = "0:/";
	
	printf(SHELL_BANNER",%s,v%d��\n", __DATE__, SHELL_VERSION);
	printf("����help��ȡ����.\n");
	
	/* Loop to get char */
	while(1)
	{
		int ch;

		/* ��ȡ��ǰ·�� */

		/* ��ӡ��ʾ�� */
		printf("[%s]#", current_path);
		ch = read_input(line_buffer, MAX_LINE_SIZE);
		printf("\n");
 
		/* �س�����ִ������ */
		if (ch == SHELL_KEY_ENTER)
		{
			execute_cmd(line_buffer);	
		}		
	}

	return 0;
}

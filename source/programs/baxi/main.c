/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

/**
	
	系统上的简单控制台
	Wuxin (82828068@qq.com)
 */

#include <types.h>
#include <stdio.h>
#include <string.h>
#include "cmd/cmd.h"

#define SHELL_BANNER "Grid操作系统命令行"
#define SHELL_VERSION 1
#define SHELL_KEY_ENTER 0x0d
#define MAX_LINE_SIZE 1024									// 一行最多的字节
static char line_buffer[MAX_LINE_SIZE];

/**
	@brief 从命令行上读取输入
	
	以回车键，CTRL+C 退出.
	
	@return
		返回退出时是哪个键
*/	
static int read_input(char * line_buffer, int max_size)
{
	int cur_pos = 0;
	
	line_buffer[0] = 0;
	do
	{
		/* 读取字符 */
		int ch = getch();
		
		/* 弹起？忽略 */
		if (ch == 0) continue;

		/* 是一个完整的按键吗？ 方向键有多个 */

		/* 是退出类别的键吗？*/
		if (ch == SHELL_KEY_ENTER)
		{			
			return ch;
		}

		/* 最后存入到缓冲区 */
		line_buffer[cur_pos++] = ch;
		line_buffer[cur_pos] = 0;
		putchar(ch);

	} while (cur_pos < max_size - 1/*end code */);

	/* 此时已近达到最大缓冲区能容下的字符数，那么此时不能再接收了，该响个铃？*/
	while (1)
	{
		if (getch() == SHELL_KEY_ENTER)
			return SHELL_KEY_ENTER;
		else
		{
			/* 响铃 */
		}
	}
}

/**
	@brief 执行命令
*/
static void execute_cmd(char * line_buffer)
{
	void * cmd;

	/* 立马按了回车键 */
	if (!strlen(line_buffer)) return;

// 	printf("执行命令：%s(%d个字节)。\n", line_buffer, max_size);
	cmd = command_find(line_buffer);
	if (!cmd)
	{
		printf("没有找到您输入的命令。\n");
		return;
	}

	if (command_exec(cmd, line_buffer))
	{
		printf("命令执行失败。\n");
	}
}

void main(int argc, char * argv[])
{	
	char current_path[] = "0:/";
	printf(SHELL_BANNER"v%d.\n", SHELL_VERSION);
	
	/* Loop to get char */
	while(1)
	{
		int ch;

		/* 获取当前路径 */

		/* 打印提示符 */
		printf("[%s]#", current_path);
		ch = read_input(line_buffer, MAX_LINE_SIZE);
		printf("\n");
 
		/* 回车键则执行命令 */
		if (ch == SHELL_KEY_ENTER)
		{
			execute_cmd(line_buffer);	
		}		
	}
}

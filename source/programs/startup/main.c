/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *
 *   系统初始化进程
 */

#include <stdio.h>

int test_bss_data;

void main()
{
	FILE *fp;
	
	printf("初始化进程启动中(stack = %p)...\n", &fp);
	
	fp = fopen("0:/abc", "r");
	printf("Test open result = %p\n", fp);
}
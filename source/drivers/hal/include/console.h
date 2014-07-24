/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   LuYan
*   HAL Console
*/

#ifndef HAL_CONSOLE_H
#define HAL_CONSOLE_H

#include <ddk/compatible.h>
#include <screen.h>

/* ѭ������,�ݴ浱ǰ��Ļ������� */
#define CONSOLE_BUFFER_SIZE 4 * 1024
struct hal_console_buffer
{
	/*                              |<----------|                               */
	/* [pre_line_idx][next_line_idx]text1"\n"[pre_line][next_line_idx]text2"\n" */
	/*                |----------------------------------------------->|        */
	unsigned char buf[CONSOLE_BUFFER_SIZE];

	/* ��һ����Ч�����ַ��±� */
	unsigned int  topline;

	/* ���һ���ַ����±� */
	unsigned int lastchar;

	/* ������Ŀǰû����ʾ����\n�������±� */
	unsigned int tail_line;
};

/* ��ʾ��������ǰ������ʾ�ġ����ڡ� */
struct hal_console_window
{
	/* ��ǰ��Ļ�ϵ�һ���ַ������û����롰�С������ַ��±� */
	unsigned int  topline;	

	/* ��ǰ�������һ���ַ����±� */
	unsigned int  lastchar;	

	unsigned int pos_x;
	unsigned int pos_y;	

	/* 0:���ڴ��ڻ���ĵײ���1:���ڴ��ڻع�״̬ */
	unsigned int downward;
	/* ��¼��ǰ״̬�£���Ļ�Ϲ�ʱ���µ������ڻ����е�λ�ã�step[0]Ϊ���й�����step[1]ΪPGDN */
	int step[2];
};

struct hal_console_context
{
	int max_x, max_y;	/* �ֱ��� */
	struct hal_console_window window;
	struct hal_console_buffer buffer;	
	unsigned int color;
	spinlock_t lock;
};


struct hal_console_ops
{
	int (*read)(char *buffer, int size);
	int (*write)(char *buffer, int size);
};
static inline void get_screen_resolution(int *w, int *h, int *bpp)
{
	*w = main_screen.width;
	*h = main_screen.height;
}

void console_write(char *string, int size);
void hal_console_init(void);

#endif
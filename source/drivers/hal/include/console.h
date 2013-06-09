/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   LuYan
*   HAL Console
*/

#ifndef HAL_CONSOLE_H
#define HAL_CONSOLE_H

//#include <ddk/compatible.h>

/* 循环缓存,暂存当前屏幕输出数据 */
#define CONSOLE_BUFFER_SIZE 4 * 1024
struct hal_console_buffer
{
	/*                              |<----------|                               */
	/* [pre_line_idx][next_line_idx]text1"\n"[pre_line][next_line_idx]text2"\n" */
	/*                |----------------------------------------------->|        */
	unsigned char buf[CONSOLE_BUFFER_SIZE];

	/* 第一行有效行首字符下标 */
	unsigned int  topline;

	/* 最后一个字符的下标 */
	unsigned int lastchar;

	/* 缓存中目前没有显示输入\n的行首下标 */
	unsigned int tail_line;

	/* 锁 */
	//spinlock_t lock;
};

/* 显示，描述当前正在显示的“窗口” */
struct hal_console_window
{
	/* 当前屏幕上第一个字符所属用户输入“行”的首字符下标 */
	unsigned int  topline;	

	/* 当前窗口最后一个字符的下标 */
	unsigned int  lastchar;	

	unsigned int pos_x;
	unsigned int pos_y;	

	/* 锁 */
	//spinlock_t lock;
	/* 0:窗口处于缓存的底部，1:窗口处于回滚状态 */
	unsigned int downward;
	/* 记录当前状态下，屏幕上滚时，新的首行在缓存中的位置，step[0]为单行滚动，step[1]为PGDN */
	int step[2];
};

struct hal_console_context
{
	int max_x, max_y;	/* 分辨率 */
	struct hal_console_window window;
	struct hal_console_buffer buffer;	
	unsigned int color;
//	spinlock_t lock;
};


struct hal_console_ops
{
	int (*read)(char *buffer, int size);
	int (*write)(char *buffer, int size);
};

static inline void get_screen_resolution(int *w, int *h, int *bpp)
{
	*w = 1024;
	*h = 768;
}

void console_write(char *string, int size);
void hal_console_init(void);

#endif
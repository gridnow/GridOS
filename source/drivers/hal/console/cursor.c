/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL Video soft cursor
*/
#include <types.h>
#include <screen.h>

#include <ddk/compatible.h>

static struct timer_list cursor_timer;
static int is_show, cursor_stoped;
static int cursor_not_inited = 1;
static int cursor_height, cursor_width, cursor_x, cursor_y;
static void video_draw_cursor(unsigned int x, unsigned int y, unsigned int length, unsigned int width, unsigned int clr)
{
	unsigned int	i;
	unsigned int	j;	
	unsigned int	end_x =(x + width);
	unsigned int	end_y =(y + length);

	for (i = x; i < end_x; i++)
	{
		for (j = y; j < end_y; j++)
		{
			video_draw_pixel(i, j, clr);	
		}
	}
}

static void black_cursor()
{
	video_draw_cursor(cursor_x, cursor_y, cursor_height, cursor_width, RGB(0, 0, 0));
}

static void timer_fuc(unsigned long unused)
{
	/* 关闭了？ */
	if (cursor_stoped == true)
		return;

	if (is_show)
	{
		is_show = 0;
		video_draw_cursor(cursor_x, cursor_y, cursor_height, cursor_width, RGB(255, 255, 255));
	}
	else
	{
		is_show = 1;
		black_cursor();
	}

	hal_timer_mod(&cursor_timer, hal_get_tick() + hal_get_tick_rate() / 4);
}

static void setup_timer()
{
	hal_timer_init(&cursor_timer, 0, NULL);

	cursor_timer.function = timer_fuc;
	cursor_timer.expires = hal_get_tick() + 1 * hal_get_tick_rate();		
	hal_timer_add(&cursor_timer);
}

void console_cursor_set_height(int h)
{
	cursor_height = h;
}
 
/**
	@brief 移动光标

	一般在文字输出的时候移动, 建议在关闭光标后移动光标
*/
void console_cursor_move(unsigned int x, unsigned int y)
{
	cursor_x = x;
	cursor_y = y;
}

/**
	@brief 使能光标

	一般在文字输出的时候停止，在文字输出完毕后开启
*/
void console_cursor_toggle(int enable)
{
	if (enable)
	{
		/* The console may be used very ealy before the timer module startup! */
		if (cursor_not_inited == true)
			return;

		cursor_stoped = false;
		hal_timer_mod(&cursor_timer, hal_get_tick() + hal_get_tick_rate() / 4);
	}
	else
	{
		hal_timer_del(&cursor_timer);
		cursor_stoped = true;

		/* Clear it */
		black_cursor();	
	}
}

/**
	@brief 初始化光标

	设置光标定时器，应该在系统定时器初始化完毕后使用
*/
__init void console_cursor_setup()
{
	/* The console may be initialized twice due to the arch */
	if (cursor_not_inited)
	{
		cursor_width = 10;
		cursor_not_inited = 0;
		setup_timer(); 
	}
}

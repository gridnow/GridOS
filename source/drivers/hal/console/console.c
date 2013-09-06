/**
 *   Copyright 2012,2013
 *
 *   Yan.lu
 *   图形模式的CONSOLE后端
*/

#include <string.h>

#include <console.h>
#include <screen.h>
#include "internal.h"

#define DOTFNT_CHAR_SPACE_LEN	6		//pixel length for " "
#define DOTFNT_CHAR_LINE_HEIGHT	15		//pixel count of the HEIGHT of a line
#define DOTFNT_ENG_CHAR_WIDTH	16		//用于断行

#define WHITE_PIX -1
#define BLACK_PIX 0

#define PGDN_STEP 5
#define PGUP_STEP 5

/* 得到循环缓存中idx对应字节的地址 */
#define get_idx_addr_in_buf(idx)	((char*)( (unsigned long)dsp_ctx.buffer.buf + (idx) ))
/* 得到循环缓冲区下的index */
#define get_buf_idx(idx) ((idx) & (CONSOLE_BUFFER_SIZE - 1))
/* 计算缓存中head至tail之间的空间长度 */
#define get_buf_length(head,tail) ((tail - head + CONSOLE_BUFFER_SIZE) & (CONSOLE_BUFFER_SIZE - 1))


static struct hal_console_context dsp_ctx;

/*
	将字符c写入缓冲区末尾 
*/
static inline void write_to_buf(unsigned char c)
{
	char* lastchar = get_idx_addr_in_buf(dsp_ctx.buffer.lastchar);
	*lastchar = c;
	dsp_ctx.buffer.lastchar = get_buf_idx(dsp_ctx.buffer.lastchar + 1);

	return;
}

/* 以下函数更新、读取缓存状态 */

static inline void set_line_prev_idx(unsigned int line_idx)
{
	int i = 0;
	char *c;
	
	
	c = (char*)&line_idx;

	for (i = 0; i < sizeof(unsigned int); i++ )
	{		
		write_to_buf(*c);
		c++;
	}		

	return;
}

static inline void set_line_next_idx(unsigned int next_line_idx_idx, unsigned int line_idx)
{
	int i = 0;
	char *c;
	
	
	c = (char*)&line_idx;

	for (i = 0; i < sizeof(unsigned int); i++ )
	{		
		char* idx_addr = get_idx_addr_in_buf(next_line_idx_idx);
		*idx_addr = *c;
		next_line_idx_idx = get_buf_idx(next_line_idx_idx + 1);
		c++;
	}		

	return;
}

static unsigned int get_line_idx(unsigned int current_idx)
{
	unsigned int line_idx;
	char* line_idx_addr;
	char* c;
	int i;

	c = (char*)&line_idx;
	for (i = 0; i < sizeof(unsigned int); i++)
	{
		line_idx_addr = get_idx_addr_in_buf(current_idx);
		*c = *line_idx_addr;
		current_idx = get_buf_idx(current_idx + 1);
		c++;
	}

	return line_idx;
}

static unsigned int prev_line_in_buffer(unsigned int current_idx)
{
	/* 前一行的index */
	unsigned int prev_line_idx_idx = get_buf_idx(current_idx - (2 * sizeof(unsigned int)) );	

	return get_line_idx(prev_line_idx_idx);
}

static unsigned int next_line_in_buffer(unsigned int current_idx)
{
	unsigned int next_line_idx_idx = get_buf_idx(current_idx - sizeof(unsigned int));
		
	return get_line_idx(next_line_idx_idx);
}

static unsigned int cp_to_buffer(char* buf)
{
	char *p_string = buf;
	unsigned int next_line_idx_idx;
	
	while (*p_string != '\0')
	{
		write_to_buf(*p_string);
		if (*p_string == '\n' )
		{
			set_line_prev_idx(dsp_ctx.buffer.tail_line);
			dsp_ctx.buffer.lastchar = get_buf_idx(dsp_ctx.buffer.lastchar + sizeof(unsigned int));
			next_line_idx_idx = get_buf_idx(dsp_ctx.buffer.tail_line - sizeof(unsigned int));
			set_line_next_idx(next_line_idx_idx, dsp_ctx.buffer.lastchar);
			dsp_ctx.buffer.tail_line = dsp_ctx.buffer.lastchar;
			p_string++;
			break;
		}
		p_string++;	
	}
	return (unsigned int)((unsigned long)p_string - (unsigned long)buf);
}
/*   */


/*更新窗口状态*/
static void reset_up_idx()
{
	dsp_ctx.window.step[0] = -1;
	dsp_ctx.window.step[1] = -1;
	return;
}

static void modify_up_idx(unsigned int pos_x, unsigned int pos_y)
{	
	if (pos_x == 0)
	{
		if (pos_y == DOTFNT_CHAR_LINE_HEIGHT)
		{
			dsp_ctx.window.step[0] = dsp_ctx.window.lastchar;
		}
		else if (pos_y == DOTFNT_CHAR_LINE_HEIGHT * PGDN_STEP)
		{
			dsp_ctx.window.step[1] = dsp_ctx.window.lastchar;
		}
	}
	return;
}


/* 向屏幕打印缓存中的字符 */
static int draw_a_line(unsigned int* pos_x, unsigned int* pos_y, unsigned int start, unsigned int end)
{
	u16 gbk_code;
	unsigned char *c, *byte2;	
	int lf = 0;
	int count = 0;

	unsigned int p_string	= get_buf_idx(start);
	unsigned int color		= dsp_ctx.color;
	unsigned int cur_x		= *pos_x;
	unsigned int cur_y		= *pos_y;
	

	/* Disable the cursor while we are drawing */
	console_cursor_toggle(false);

	while (p_string != end)  
	{

		//c = *p_string;
		c = (unsigned char*)get_idx_addr_in_buf(p_string);

		if (*c == ' ')
		{
			cur_x += DOTFNT_CHAR_SPACE_LEN;			
			p_string = get_buf_idx(p_string + 1);			
		}
		else if (*c == '\n')
		{
			cur_x = 0;
			lf = 1;
			p_string = get_buf_idx(p_string + 1);
		}		
		else if (*c == 0xd)
		{
			p_string = get_buf_idx(p_string + 1);
			//cur_x = 0;			
		}
		else if (*c == 0x09/*tab*/)
		{
			p_string = get_buf_idx(p_string + 1);
			cur_x |= 31;
			cur_x++;
		}
		/* English is 1 byte */
		else if (*c >= 33 && *c < 127)
		{			
			cur_x += draw_eng_char(*c, cur_x, cur_y, color);
			p_string = get_buf_idx(p_string + 1);	
		}
		/* Chinese is 2 bytes */
		else
		{
			byte2 = get_idx_addr_in_buf(get_buf_idx(p_string + 1));
			gbk_code = (*c << 8) | (*byte2);			
			cur_x += draw_gbk_chinese(gbk_code, cur_x, cur_y, color);			
			p_string = get_buf_idx(p_string + 2);
		}

		/* Modify position,check overflow */
		if (cur_x + DOTFNT_ENG_CHAR_WIDTH >= dsp_ctx.max_x)
		{
			cur_x = 0;		
		}
		if (cur_x == 0)			/* 已经打印完一行(屏幕)，退出 */
		{										
			cur_y += DOTFNT_CHAR_LINE_HEIGHT;
			break;
		}		
	}

	*pos_x = cur_x;
	*pos_y = cur_y;

	/* Set the cursor position before enable cursor */
	console_cursor_move(cur_x, cur_y);
	console_cursor_toggle(true);

	count = get_buf_length(start, p_string);
	/* 遇到\n，需要跨过缓存中保存的index */
	if (lf)
	{
		count += 2 * sizeof(unsigned int);
	}
	
	return count;
}

/*
	屏幕向下滚动 注意，此函数不是接口，实现这个功能还需要在外面包一层
    目前接口还未实现，因为目前还不具备所需的条件
*/
static void erase_dnward_console_screen(int step)
{
	int pos_x, pos_y;
	int i;
	unsigned long newline, erased_line;
	unsigned long erased_count, count;
	int erased_pos_x = 0; 
	int erased_pos_y = 0;
	int new_pos_x = 0;
	int new_pos_y = 0;
		
	/* 
		当前已经是第一行，直接退出 
	*/
	if (dsp_ctx.window.topline == dsp_ctx.buffer.topline)
	{
		return;
	}
	
	reset_up_idx();
	erased_line = dsp_ctx.window.topline;

	/* 
		定位屏幕应输出的第一行 
	*/
	newline = erased_line;
	for (i = 0; i < step; i++)
	{
		newline = prev_line_in_buffer(newline);
		if (newline == dsp_ctx.buffer.topline)
			break;	
	}

	dsp_ctx.window.topline = newline;
	/*
		刷新屏幕，每次循环刷新一行
		从屏幕左上角开始刷新
	*/
	while (1)
	{		
		if (erased_line != dsp_ctx.buffer.lastchar)
		{			
			dsp_ctx.color = BLACK_PIX;					/* 用黑色重写本行，即擦除本行 */			
			erased_count = draw_a_line(&erased_pos_x, &erased_pos_y, erased_line, dsp_ctx.buffer.lastchar);
			erased_line = get_buf_idx(erased_line + erased_count);
		}
				
		/* 
			向屏幕打印需输出的行 
		*/
		dsp_ctx.color = WHITE_PIX;		
		count = draw_a_line(&new_pos_x, &new_pos_y, newline, dsp_ctx.buffer.lastchar);
		dsp_ctx.window.lastchar = get_buf_idx(newline + count);		

		newline = get_buf_idx(newline + count);

		/* 新数据已经输出到缓存末尾 */
		if (newline == dsp_ctx.buffer.lastchar)
			break;

		modify_up_idx(new_pos_x, new_pos_y);
						
		/* 已打印满一屏 */
		if (new_pos_y >= (dsp_ctx.max_y - 20))	
			break;
	}

	dsp_ctx.window.downward = 1;
	dsp_ctx.window.pos_x = new_pos_x;
	dsp_ctx.window.pos_y = new_pos_y;

	dsp_ctx.color = WHITE_PIX;

	return;
}

/*
	屏幕向上滚动 函数不检查状态变化的合法性，是否处于可滚动状态由调用者判断
	
	因此，此函数不要直接调用
*/
static void erase_upward_console_screen(unsigned int new_line_idx/*, bool rolling_one_line*/)
{

	unsigned int new_line = new_line_idx; 
	unsigned int erased_line = dsp_ctx.window.topline;
	unsigned int erased_pos_x = 0; 
	unsigned int erased_pos_y = 0;
	unsigned int new_pos_x = 0;
	unsigned int new_pos_y = 0;
	unsigned int count, erased_count;
	int i;

	reset_up_idx();		
	dsp_ctx.window.topline = new_line;

	while(1)
	{
		dsp_ctx.color = BLACK_PIX;
		erased_count = draw_a_line(&erased_pos_x, &erased_pos_y, erased_line, dsp_ctx.buffer.lastchar); 
				
		/* 
			向屏幕打印替代的行 
		*/
		if (new_line != dsp_ctx.buffer.lastchar)
		{
			dsp_ctx.color = WHITE_PIX;						
			
			count = draw_a_line(&new_pos_x, &new_pos_y, new_line, dsp_ctx.buffer.lastchar);
			dsp_ctx.window.lastchar = get_buf_idx(new_line + count);			

			new_line = get_buf_idx(new_line + count);
			
			if ( new_line == dsp_ctx.buffer.lastchar)	
			{
				dsp_ctx.window.downward = 0;
			}
			else
			{
				modify_up_idx(new_pos_x, new_pos_y);
			}
		}

		/* 屏幕刷新完毕 */						
		if (erased_pos_y >= dsp_ctx.max_y - 20)
			break;										

		erased_line = get_buf_idx(erased_line + erased_count);
	}

	/* 
		维护相关数据
	*/
	dsp_ctx.window.pos_x = new_pos_x;
	dsp_ctx.window.pos_y = new_pos_y;		
	
	dsp_ctx.color = WHITE_PIX;

	return;

}


/* 
	入口函数
*/
static int write_string(char *buffer, int size)
{
	unsigned count = 0;
	unsigned int pos = 0;
	unsigned long flags;
	/* 数据长度超过缓存，直接舍弃 */
	if (strlen(buffer) + 2 * sizeof(unsigned int) >= CONSOLE_BUFFER_SIZE)
	{
		goto ret;
	}

	/* 涉及两个全局资源，无法实现并发 只能加全局大锁 */
	spin_lock_irqsave(&dsp_ctx.lock, flags);

	/* 第一步，将数据复制到缓存中,更新缓存的状态 */			
	/*
	   如果空间已经用完，则从头部循环使用
	   循环保证有足够的空间
	*/
	while(1)
	{
		while (strlen(buffer) + get_buf_length(dsp_ctx.buffer.topline, dsp_ctx.buffer.lastchar) + (2 * sizeof(unsigned int))
					>= CONSOLE_BUFFER_SIZE)
		{				
			/* 刷掉缓存中的第一行数据(以输入的\n界定，不是以显示换行界定) */
			dsp_ctx.buffer.topline = next_line_in_buffer(dsp_ctx.buffer.topline);
		
			/* 如果清空的数据正好处于当前显示窗口中，则需要重新绘制屏幕 */
			if (dsp_ctx.window.topline == dsp_ctx.buffer.topline)				
				erase_upward_console_screen(dsp_ctx.buffer.topline);				
		}
		/* 复制一行数据至缓存，行以输入的"\n"界定 */
		pos = cp_to_buffer(buffer);
		if (pos == strlen(buffer))
		{
			break;
		}

		buffer = (char*)((unsigned long)buffer + pos);
	}

	/* 第二步，向屏幕输出，更新屏幕状态 */
	/* 
		如果当前屏幕处在buf的末端，向屏幕打印新输入的字符串 并滚动
		如果当前屏幕处于回滚状态 正在显示之前的内容，则不向屏幕打印新写入的数据
	*/	
	if (dsp_ctx.window.downward == 0)
	{
		/*
			向屏幕打印新写入的数据，直到打印到最后一个字符
		*/
		while (dsp_ctx.window.lastchar != dsp_ctx.buffer.lastchar)
		{
			/* 
				屏幕满，向上滚动 
			*/
			if (dsp_ctx.window.pos_y >= dsp_ctx.max_y - 20)
			{
				erase_upward_console_screen(dsp_ctx.window.step[0]);
			}
			
			/* 屏幕不满时一行行输出 */
			else 
			{		
				/*打印一行(指屏幕)数据，或者不足一行时打印到缓存最后一个字符 count为本次处理的字节数 */
				count = draw_a_line(&dsp_ctx.window.pos_x, &dsp_ctx.window.pos_y,
									dsp_ctx.window.lastchar, 
									dsp_ctx.buffer.lastchar);
				/* 更新窗口的状态 */
				dsp_ctx.window.lastchar = get_buf_idx(dsp_ctx.window.lastchar + count);
				modify_up_idx(dsp_ctx.window.pos_x, dsp_ctx.window.pos_y);
			}						
		}
	}

	spin_unlock_irqrestore(&dsp_ctx.lock, flags);
ret:
	return;
}

/**
	@brief	接口，PGDN事件处理函数,测试条件不足，因此目前未测试
	
	@return void
*/
void upward_screen()
{
	/*if (dsp_ctx.window.step[1] == -1)
	{
		return;
	}

	if ()
	{
	}

	erase_upward_console_screen(dsp_ctx.window.step[1]);*/
}

/**
	@brief	接口，PGUP事件处理函数，测试条件不足，因此目前未实现
	
	@return void
*/
void dnward_screen()
{
	
}

/**
	@brief		 接口，分辨率变更处理函数, 环境不足，因此目前未测试
	@width[in]	 
	@height[in]	 
	@return void
*/
void modify_screen_resolution(unsigned long width, unsigned long height)
{
	int count = 0;

	/* 重绘屏幕 且屏幕首行保持不变 */
	dsp_ctx.max_x = width;
	dsp_ctx.max_y = height;
	
	dsp_ctx.window.pos_x = 0;
	dsp_ctx.window.pos_y = 0;

	reset_up_idx();
	dsp_ctx.window.lastchar = get_buf_idx(dsp_ctx.window.topline);

	while (1)
	{
		/* 
			屏幕满，向上滚动 
		*/
		if (dsp_ctx.window.pos_y >= dsp_ctx.max_y - 20)
		{
			break;
		}

		if (dsp_ctx.window.lastchar == dsp_ctx.buffer.lastchar)
		{
			dsp_ctx.window.downward = 0;			
			break;
		}		
		else 
		{		
			/*打印一行(指屏幕)数据，或者不足一行时打印到缓存最后一个字符 count为本次处理的字节数 */
			count = draw_a_line(&dsp_ctx.window.pos_x, &dsp_ctx.window.pos_y,
								dsp_ctx.window.lastchar, 
								dsp_ctx.buffer.lastchar);
			/* 更新窗口的状态 */
			dsp_ctx.window.lastchar = get_buf_idx(dsp_ctx.window.lastchar + count);
			modify_up_idx(dsp_ctx.window.pos_x, dsp_ctx.window.pos_y);
		}						
	}

	return;
}

static int noop_write(char *string, int size)
{
	return 0;
}

struct hal_console_ops video_console_ops = {
	.write = noop_write,
};

void hal_console_init(void)
{
	video_console_ops.write = write_string;
	
	dsp_ctx.window.topline = 2 * sizeof(unsigned int);
	dsp_ctx.window.pos_x = 0;
	dsp_ctx.window.pos_y = 0;
	dsp_ctx.window.lastchar = 2 * sizeof(unsigned int);
	dsp_ctx.window.downward = 0;
	dsp_ctx.window.step[0] = -1;
	dsp_ctx.window.step[1] = -1;

	/* fix me 目前仅静态分配 */
	memset(&dsp_ctx.buffer.buf, 0, CONSOLE_BUFFER_SIZE);

	dsp_ctx.color = WHITE_PIX;
	dsp_ctx.buffer.topline = 2 * sizeof(unsigned int);
	dsp_ctx.buffer.lastchar = 2 * sizeof(unsigned int); /*  */	
	dsp_ctx.buffer.tail_line = 2 * sizeof(unsigned int);
	reset_up_idx();

	/* Init the global lock */
	spin_lock_init(&dsp_ctx.lock);

	/* 取分辨率 */
	get_screen_resolution(&dsp_ctx.max_x, &dsp_ctx.max_y, NULL); 

	console_cursor_setup();
	console_cursor_set_height(DOTFNT_CHAR_LINE_HEIGHT);

	return;
}

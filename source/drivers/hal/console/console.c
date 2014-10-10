/**
 *   Copyright 2012,2013
 *
 *   Yan.lu
 *   ͼ��ģʽ��CONSOLE���
*/

#include <string.h>

#include <console.h>
#include <screen.h>
#include "internal.h"

#define DOTFNT_CHAR_SPACE_LEN	6		//pixel length for " "
#define DOTFNT_CHAR_LINE_HEIGHT	15		//pixel count of the HEIGHT of a line
#define DOTFNT_ENG_CHAR_WIDTH	16		//���ڶ���

#define WHITE_PIX 0xffffffff
#define BLACK_PIX 0

#define PGDN_STEP 5
#define PGUP_STEP 5

/* �õ�ѭ��������idx��Ӧ�ֽڵĵ�ַ */
#define get_idx_addr_in_buf(idx)	((char*)( (unsigned long)dsp_ctx.buffer.buf + (idx) ))
/* �õ�ѭ���������µ�index */
#define get_buf_idx(idx) ((idx) & (CONSOLE_BUFFER_SIZE - 1))
/* ���㻺����head��tail֮��Ŀռ䳤�� */
#define get_buf_length(head,tail) ((tail - head + CONSOLE_BUFFER_SIZE) & (CONSOLE_BUFFER_SIZE - 1))


static struct hal_console_context dsp_ctx;

/*
	���ַ�cд�뻺����ĩβ 
*/
static inline void write_to_buf(unsigned char c)
{
	char* lastchar = get_idx_addr_in_buf(dsp_ctx.buffer.lastchar);
	*lastchar = c;
	dsp_ctx.buffer.lastchar = get_buf_idx(dsp_ctx.buffer.lastchar + 1);

	return;
}

/* ���º������¡���ȡ����״̬ */

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
	/* ǰһ�е�index */
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


/*���´���״̬*/
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


/* ����Ļ��ӡ�����е��ַ� */
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
		/*127�˸��,27�ĸ������,131-138,255��F3-10 F12,140��F11*/
		else if (*c == 127 || *c == 27 || *c == 131 ||
			*c == 255 || *c == 132 || *c == 133 || 
			*c == 134 || *c == 135 || *c == 136 || 
			*c == 137 || *c == 138 || *c == 140){
			p_string = get_buf_idx(p_string + 1);/*��1 �����������ǣ���Щ���˵ļ���ֱ���������Ͳ�������*/
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
		if (cur_x == 0)			/* �Ѿ���ӡ��һ��(��Ļ)���˳� */
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
	/* ����\n����Ҫ��������б����index */
	if (lf)
	{
		count += 2 * sizeof(unsigned int);
	}
	
	return count;
}

/*
	��Ļ���¹��� ע�⣬�˺������ǽӿڣ�ʵ��������ܻ���Ҫ�������һ��
    Ŀǰ�ӿڻ�δʵ�֣���ΪĿǰ�����߱����������
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
		��ǰ�Ѿ��ǵ�һ�У�ֱ���˳� 
	*/
	if (dsp_ctx.window.topline == dsp_ctx.buffer.topline)
	{
		return;
	}
	
	reset_up_idx();
	erased_line = dsp_ctx.window.topline;

	/* 
		��λ��ĻӦ����ĵ�һ�� 
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
		ˢ����Ļ��ÿ��ѭ��ˢ��һ��
		����Ļ���Ͻǿ�ʼˢ��
	*/
	while (1)
	{		
		if (erased_line != dsp_ctx.buffer.lastchar)
		{			
			dsp_ctx.color = BLACK_PIX;					/* �ú�ɫ��д���У����������� */			
			erased_count = draw_a_line(&erased_pos_x, &erased_pos_y, erased_line, dsp_ctx.buffer.lastchar);
			erased_line = get_buf_idx(erased_line + erased_count);
		}
				
		/* 
			����Ļ��ӡ��������� 
		*/
		dsp_ctx.color = WHITE_PIX;		
		count = draw_a_line(&new_pos_x, &new_pos_y, newline, dsp_ctx.buffer.lastchar);
		dsp_ctx.window.lastchar = get_buf_idx(newline + count);		

		newline = get_buf_idx(newline + count);

		/* �������Ѿ����������ĩβ */
		if (newline == dsp_ctx.buffer.lastchar)
			break;

		modify_up_idx(new_pos_x, new_pos_y);
						
		/* �Ѵ�ӡ��һ�� */
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
	��Ļ���Ϲ��� ���������״̬�仯�ĺϷ��ԣ��Ƿ��ڿɹ���״̬�ɵ������ж�
	
	��ˣ��˺�����Ҫֱ�ӵ���
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
			����Ļ��ӡ������� 
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

		/* ��Ļˢ����� */						
		if (erased_pos_y >= dsp_ctx.max_y - 20)
			break;										

		erased_line = get_buf_idx(erased_line + erased_count);
	}

	/* 
		ά���������
	*/
	dsp_ctx.window.pos_x = new_pos_x;
	dsp_ctx.window.pos_y = new_pos_y;		
	
	dsp_ctx.color = WHITE_PIX;

	return;

}


/* 
	��ں���
*/
static int write_string(char *buffer, int size)
{
	unsigned count = 0;
	unsigned int pos = 0;
	unsigned long flags;
	/* ���ݳ��ȳ������棬ֱ������ */
	if (strlen(buffer) + 2 * sizeof(unsigned int) >= CONSOLE_BUFFER_SIZE)
	{
		goto ret;
	}

	/* �漰����ȫ����Դ���޷�ʵ�ֲ��� ֻ�ܼ�ȫ�ִ��� */
	spin_lock_irqsave(&dsp_ctx.lock, flags);

	/* ��һ���������ݸ��Ƶ�������,���»����״̬ */			
	/*
	   ����ռ��Ѿ����꣬���ͷ��ѭ��ʹ��
	   ѭ����֤���㹻�Ŀռ�
	*/
	while(1)
	{
		while (strlen(buffer) + get_buf_length(dsp_ctx.buffer.topline, dsp_ctx.buffer.lastchar) + (2 * sizeof(unsigned int))
					>= CONSOLE_BUFFER_SIZE)
		{				
			/* ˢ�������еĵ�һ������(�������\n�綨����������ʾ���н綨) */
			dsp_ctx.buffer.topline = next_line_in_buffer(dsp_ctx.buffer.topline);
		
			/* �����յ��������ô��ڵ�ǰ��ʾ�����У�����Ҫ���»�����Ļ */
			if (dsp_ctx.window.topline == dsp_ctx.buffer.topline)				
				erase_upward_console_screen(dsp_ctx.buffer.topline);				
		}
		/* ����һ�����������棬���������"\n"�綨 */
		pos = cp_to_buffer(buffer);
		if (pos == strlen(buffer))
		{
			break;
		}

		buffer = (char*)((unsigned long)buffer + pos);
	}

	/* �ڶ���������Ļ�����������Ļ״̬ */
	/* 
		�����ǰ��Ļ����buf��ĩ�ˣ�����Ļ��ӡ��������ַ��� ������
		�����ǰ��Ļ���ڻع�״̬ ������ʾ֮ǰ�����ݣ�������Ļ��ӡ��д�������
	*/	
	if (dsp_ctx.window.downward == 0)
	{
		/*
			����Ļ��ӡ��д������ݣ�ֱ����ӡ�����һ���ַ�
		*/
		while (dsp_ctx.window.lastchar != dsp_ctx.buffer.lastchar)
		{
			/* 
				��Ļ�������Ϲ��� 
			*/
			if (dsp_ctx.window.pos_y >= dsp_ctx.max_y - 20)
			{
				erase_upward_console_screen(dsp_ctx.window.step[0]);
			}
			
			/* ��Ļ����ʱһ������� */
			else 
			{		
				/*��ӡһ��(ָ��Ļ)���ݣ����߲���һ��ʱ��ӡ���������һ���ַ� countΪ���δ�����ֽ��� */
				count = draw_a_line(&dsp_ctx.window.pos_x, &dsp_ctx.window.pos_y,
									dsp_ctx.window.lastchar, 
									dsp_ctx.buffer.lastchar);
				/* ���´��ڵ�״̬ */
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
	@brief	�ӿڣ�PGDN�¼�������,�����������㣬���Ŀǰδ����
	
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
	@brief	�ӿڣ�PGUP�¼��������������������㣬���Ŀǰδʵ��
	
	@return void
*/
void dnward_screen()
{
	
}

/**
	@brief		 �ӿڣ��ֱ��ʱ��������, �������㣬���Ŀǰδ����
	@width[in]	 
	@height[in]	 
	@return void
*/
void modify_screen_resolution(unsigned long width, unsigned long height)
{
	int count = 0;

	/* �ػ���Ļ ����Ļ���б��ֲ��� */
	dsp_ctx.max_x = width;
	dsp_ctx.max_y = height;
	
	dsp_ctx.window.pos_x = 0;
	dsp_ctx.window.pos_y = 0;

	reset_up_idx();
	dsp_ctx.window.lastchar = get_buf_idx(dsp_ctx.window.topline);

	while (1)
	{
		/* 
			��Ļ�������Ϲ��� 
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
			/*��ӡһ��(ָ��Ļ)���ݣ����߲���һ��ʱ��ӡ���������һ���ַ� countΪ���δ�����ֽ��� */
			count = draw_a_line(&dsp_ctx.window.pos_x, &dsp_ctx.window.pos_y,
								dsp_ctx.window.lastchar, 
								dsp_ctx.buffer.lastchar);
			/* ���´��ڵ�״̬ */
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
	
	dsp_ctx.window.topline = 2 * sizeof(unsigned int);
	dsp_ctx.window.pos_x = 0;
	dsp_ctx.window.pos_y = 0;
	dsp_ctx.window.lastchar = 2 * sizeof(unsigned int);
	dsp_ctx.window.downward = 0;
	dsp_ctx.window.step[0] = -1;
	dsp_ctx.window.step[1] = -1;

	/* fix me Ŀǰ����̬���� */
	memset(&dsp_ctx.buffer.buf, 0, CONSOLE_BUFFER_SIZE);

	dsp_ctx.color = WHITE_PIX;
	dsp_ctx.buffer.topline = 2 * sizeof(unsigned int);
	dsp_ctx.buffer.lastchar = 2 * sizeof(unsigned int); /*  */	
	dsp_ctx.buffer.tail_line = 2 * sizeof(unsigned int);
	reset_up_idx();

	/* Init the global lock */
	spin_lock_init(&dsp_ctx.lock);

	/* ȡ�ֱ��� */
	get_screen_resolution(&dsp_ctx.max_x, &dsp_ctx.max_y, NULL); 

	/* ��������ֱ�����0 x 0�Ļ���write_string �ƺ���bug���������ε� */
	if (dsp_ctx.max_x == 0 || dsp_ctx.max_y == 0)
		;
	else
		video_console_ops.write = write_string;
	console_cursor_setup();
	console_cursor_set_height(DOTFNT_CHAR_LINE_HEIGHT);

	return;
}

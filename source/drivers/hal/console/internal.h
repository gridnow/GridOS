#ifndef VIDEO_INTERNAL_H
#define VIDEO_INTERNAL_H

//dotfnt.c
int get_eng_char_width(unsigned char ascii);int get_chs_char_width(unsigned int gbk_code);
unsigned int draw_eng_char(unsigned char c, int org_x, int org_y, unsigned int color);
unsigned int draw_gbk_chinese(unsigned short gbk_code, int org_x, int org_y, unsigned int color);

//cursor.h
void console_cursor_toggle(int enable);
void console_cursor_setup();
void console_cursor_set_height(int h);
void console_cursor_move(unsigned int x, unsigned int y);

#endif 
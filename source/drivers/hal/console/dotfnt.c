/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL Video
*/
#include <screen.h>
#include "internal.h"
#include "12x12.c"

/* Draw the chiense gbk char return the drew pixel count */
unsigned int draw_gbk_chinese(unsigned short gbk_code, int org_x, int org_y, unsigned int color)
{
	unsigned short *gbk_dot, dr1, dr2, dr3, org_code = gbk_code;
	int k, x, y;
	unsigned char *bt;

	/*
		Note that:
		The range 0x00-0x39 is not used for representing chinese char,
		so we need to delete it in coding
	*/
	k = (gbk_code >> 8) - 0x81;	

	gbk_code -= 0x8140;															//goto the start address of the gbk table
	gbk_code -= (k * 0x41);														//本来是-0x40，上一段的GBK CODE与下一段的GBK code连接起来,(本来是断开的)。the last code of a set is not used!!!所以是-0x41
	bt = gbk_code * 18 + chs_boot_font_address;									//each char accounts to 18 bytes
	
	gbk_dot = (unsigned short*)bt;												//locate the gbk dot in the table;

	/*
		we need several techniques to do this!!!!
		gbk dot for every Chinese char is 18 bytes
		and dot is 12*12 in size.
	*/
	dr1 = gbk_dot[0];
	dr2 = gbk_dot[1];
	dr3 = gbk_dot[2];
	
	k=0;
	x=0;
	while (x < 12)
	{
		for (y = 0; y < 12; y++)
		{
			if ((dr1 & (0x8000 >> y)) != 0)
					video_draw_pixel(x + org_x, y + org_y, color);
		}		
		dr1 <<= 12;
		dr1 |= (dr2 >> 4);
		x++;
		for (y = 0; y < 12; y++)
		{
			if ((dr1 & (0x8000 >> y)) != 0)
					video_draw_pixel(x + org_x, y + org_y, color);
		}
		
		dr1 =dr2 << 8;
		dr1 |= (dr3 >> 8);
		x++;
		for (y = 0; y < 12; y++)
		{
			if ((dr1 & (0x8000 >> y)) !=0)
					video_draw_pixel(x + org_x, y + org_y, color);
		}
		
		dr1 = dr3 << 4;
		x++;
		for (y = 0; y < 12; y++)
		{
			if ((dr1 & (0x8000 >> y)) != 0)
					video_draw_pixel(x + org_x, y + org_y, color);
		}
		x++;
		k += 3;
		dr1 = gbk_dot[k];
		dr2 = gbk_dot[k + 1];
		dr3 = gbk_dot[k + 2];		
		
	}
	
	return get_chs_char_width(org_code);
}

/* Draw the ASCII code ,return the drew pixel count */
unsigned int draw_eng_char(unsigned char c, int org_x, int org_y, unsigned int color)
{
	unsigned short *linear_dot, dr1, dr2, dr3;
	unsigned char *pt;
	int k, x, y;


	/*
		note that:
		English char range: 30-127,the encoding is linear mode,each char 
		use 18 bytes
	*/
																				//get the char's offset	
																				//c is ascii,-33 is the first index,+..is the table address
	pt = (c - 33) * 32 + (char*)eng_boot_font_address;									//get the char's dot address in the table;
	linear_dot = (unsigned short*)pt;			

	/*
		we need several techniques to do this!!!!
		char is 32 bytes
		and dot is 16*16 in size.
	*/	
	k = 0;
	for (x = 0; x < 16; x++)
	{
		dr1 = linear_dot[k];
		if (dr1==0)																//不等宽字需要提前结束输出!!!
			if (x > 3)															//the first dot maybe 0,so make sure that it's at the tail
				break;
		for (y = 0; y < 16; y++)
		{
			if ((dr1 & (0x8000 >> y)) != 0)
				video_draw_pixel(x + org_x, y + org_y, color);				
		}		
		k++;
	}		
	return get_eng_char_width(c);
}

/**
	@brief Get the char width
*/
int get_eng_char_width(unsigned char ascii){	return get_chs_char_width(0) / 2;#if 0 /* 用等宽字体了 */	unsigned short *linear_dot, dr1, dr2, dr3;
	unsigned char *pt;
	int k, x, y;


	/*
		note that:
		English char range: 30-127,the encoding is linear mode,each char 
		use 18 bytes
	*/
																				//get the char's offset	
																				//c is ascii,-33 is the first index,+..is the table address
	pt = (ascii - 33) * 32 + (char*)eng_boot_font_address;									//get the char's dot address in the table;
	linear_dot = (unsigned short*)pt;			

	/*
		we need several techniques to do this!!!!
		char is 32 bytes
		and dot is 16*16 in size.
	*/	
	k = 0;
	for (x = 0; x < 16; x++)
	{
		dr1 = linear_dot[k];
		if (dr1==0)																//不等宽字需要提前结束输出!!!
			if (x > 3)															//the first dot maybe 0,so make sure that it's at the tail
				break;	
		k++;
	}		
	return x;
#endif}
/**
	@brief Get the char width
*/
int get_chs_char_width(unsigned int gbk_code){	return 14;}


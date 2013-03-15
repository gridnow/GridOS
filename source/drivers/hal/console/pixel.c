/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL Video
*/
#include <limits.h>
#include "screen.h"

#define RGB32_TO_RGB16(pixel)  ( (((pixel) & 0xF80000) >> 8) | \
	(((pixel) & 0x00FC00) >> 5) | \
	(((pixel) & 0x0000F8) >> 3) )

void video_draw_pixel(unsigned int x, unsigned int y, unsigned int clr)
{
	unsigned char	*where;
	unsigned short	*where_w;
	unsigned int	*where_d;

	/* The video not started */
	if (!main_screen.fb_base) return;

	where = (unsigned char*)(main_screen.fb_base + (((y * main_screen.width) + x) * 
		(main_screen.bpp / CHAR_BIT)));

	if (main_screen.bpp == 24 || main_screen.bpp == 32)
	{
		where[0] =(unsigned char)(clr);
		where[1] =(unsigned char)(clr >> 8);
		where[2] =(unsigned char)(clr >> 16);
	}
	else if (main_screen.bpp == 16)
	{
		where_w = (unsigned short*)where;
		where_w[0] = (unsigned short)RGB32_TO_RGB16(clr);		
	}
	else
	{
		clr = video_rgb24_to_8(clr);
		where[0] = (unsigned char)clr;
	}
}

void video_draw_bitmap(int x, int y, int width, int height, int bpp, void * user_bitmap)
{
	int start_x, start_y;
	unsigned int * bitmap = user_bitmap;

	for (start_y = 0; start_y < height; start_y++)
	{
		for (start_x = 0; start_x < width; start_x++)
		{
			video_draw_pixel(start_x + x, start_y + y, bitmap[width * start_y + start_x]);
		}
	}	
}

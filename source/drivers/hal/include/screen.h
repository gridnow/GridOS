/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL SCREEN
*/

#ifndef HAL_SCREEN_H
#define HAL_SCREEN_H

struct video_screen_info
{
	int bpp;
	int width;
	int height;
	unsigned char *fb_base, *fb_physical;
};
extern struct video_screen_info main_screen;					//main screen,defined in screen.c

//screen.c
unsigned char video_rgb24_to_8(unsigned int colour);
void video_get_screen_resolution(int *w, int *h, int *bpp);
void hal_video_init_screen();

//pixel.c
void video_draw_pixel(unsigned int x, unsigned int y, unsigned int clr);
void video_draw_bitmap(int x, int y, int width, int height, int bpp, void * user_bitmap);

//common macro
#define RGB(r,g,b) (((r)<<16)|((g)<<8)|((b)))

#endif

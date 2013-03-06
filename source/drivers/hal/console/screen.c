/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL Video
*/
#include <screen.h>

struct video_screen_info main_screen;

/**
	@brief translate the RGB to 8 bits color
*/
unsigned char video_rgb24_to_8(unsigned int colour)
{
	unsigned char red, green, blue, intensity;
#define COLOUR_RED(c)           (((c) >> 16) & 0xff)
#define COLOUR_GREEN(c)         (((c) >> 8) & 0xff)
#define COLOUR_BLUE(c)          ((c) & 0xff)

	red = COLOUR_RED(colour) >= 0x80;
	green = COLOUR_GREEN(colour) >= 0x80;
	blue = COLOUR_BLUE(colour) >= 0x80;
	intensity = (COLOUR_RED(colour) > 0xc0) || 
		(COLOUR_GREEN(colour) > 0xc0) || 
		(COLOUR_BLUE(colour) > 0xc0);

	return (intensity << 3) | ( red<< 2) | (green << 1) | blue;
}

/**
	@brief Get the main screen resolution
*/
void video_get_screen_resolution(int *w, int *h, int *bpp)
{
	if (w) *w = main_screen.width;
	if (h) *h = main_screen.height;
	if (bpp) *bpp = main_screen.bpp;
}

/**
	@brief init the screen and draw logo 
*/
void video_init_screen()
{
	
}	


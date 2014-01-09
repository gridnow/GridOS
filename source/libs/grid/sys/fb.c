/**
	The Grid Core Library
 */

/**
	Reuqest kernel for File IO
	Zhaoyu, Yaosihai
 */
#include <types.h>
#include "sys/ke_req.h"

DLLEXPORT void sys_set_pixel(int x, int y, unsigned int color)
{
	struct sysreq_misc_draw_screen req;
	req.base.req_id = SYS_REQ_KERNEL_MISC_DRAW_SCREEN;
	req.x = x;
	req.y = y;
	req.pixel.clr = color;
	req.type = SYSREQ_MISC_DRAW_SCREEN_PIXEL;
	system_call(&req); 
}

DLLEXPORT void sys_draw_screen(int x, int y, int width, int height, int bpp, void * bitmap_buffer)
{
	struct sysreq_misc_draw_screen req;
	req.base.req_id = SYS_REQ_KERNEL_MISC_DRAW_SCREEN;
	req.x = x;
	req.y = y;
	req.type = SYSREQ_MISC_DRAW_SCREEN_BITMAP;
	req.bitmap.width	= width;
	req.bitmap.height	= height;
	req.bitmap.bpp		= bpp;
	req.bitmap.buffer	= bitmap_buffer;
	system_call(&req); 
}

DLLEXPORT void sys_get_screen_resolution(int *width, int * height, int *bpp)
{
	struct sysreq_misc_draw_screen req;
	req.base.req_id = SYS_REQ_KERNEL_MISC_DRAW_SCREEN;
	req.type = SYSREQ_MISC_DRAW_GET_RESOLUTION;
	
	system_call(&req); 
	if (width) *width = req.resolution.width;
	if (height) *height = req.resolution.height;
	if (bpp) *bpp = req.resolution.bpp;
}

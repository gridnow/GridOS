/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

/**
*  @defgroup y_standard
*  @ingroup Y_Standard_Headers
*
*
*
*  @{
*/
#ifndef _Y_STANDARD_H
#define _Y_STANDARD_H

#include <compiler.h>

BEGIN_C_DECLS;

/* Should move to kernel standard typedef */
typedef unsigned long y_handle;
#define Y_INVALID_HANDLE (-1UL)

typedef enum 
{
	Y_SYNC_WAIT_RESULT_OK	= 0,
	Y_SYNC_WAIT_ABANDONED	= -1,
	Y_SYNC_WAIT_TIMEDOUT	= -2,
	Y_SYNC_WAIT_ERROR		= -3,
}y_wait_result;

/************************************************************************/
/* ����                                                                         */
/************************************************************************/
/**
	@brief ��������

	@param[in] name ������
	@param[in] cmdline ����������

	@return
		�ɹ����ؽ��̾����ʧ�ܷ���Y_INVALID_HANDLE
*/
y_handle y_process_create(xstring name, char *cmdline);


/**
	@brief �ȴ������˳�
	
	@param[in] for_who Ҫ�ȴ��Ľ��̶���
	@param[in][out] ����ΪNULL�������д����for_who���˳������һ����main�����ķ���ֵ��
*/
y_wait_result y_process_wait_exit(y_handle for_who, unsigned long * __in __out result);


/************************************************************************/
/* ͬ������                                                                     */
/************************************************************************/
#define Y_SYNC_MAX_OBJS_COUNT 64
#define Y_SYNC_WAIT_INFINITE -1 



/************************************************************************/
/* CONSOLE                                                              */
/************************************************************************/
void sys_set_pixel(int x, int y, unsigned int color);
void sys_draw_screen(int x, int y, int width, int height, int bpp, void * bitmap_buffer);
void sys_get_screen_resolution(int *width, int * height, int *bpp);

/**
	@brief ����CMDLINE
*/
int crt0_split_cmdline(char * cmdline, int max_size, int *argc, int max_argc, char **argv);

END_C_DECLS;

#endif /* _Y_STANDARD_H */

/** @} */
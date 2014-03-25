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
#include <types.h>
#include <kernel/kernel.h>

BEGIN_C_DECLS;

/* Should move to kernel standard typedef */
typedef unsigned long y_handle;
#define Y_INVALID_HANDLE (-1UL)

/************************************************************************/
/* ����                                                                         */
/************************************************************************/
typedef enum 
{
	Y_SYNC_WAIT_RESULT_OK	= 0,
	Y_SYNC_WAIT_ABANDONED	= -1,
	Y_SYNC_WAIT_TIMEDOUT	= -2,
	Y_SYNC_WAIT_ERROR		= -3,
} y_wait_result;

struct y_thread_environment_block
{
	void *self;
	void *mi;
};

struct y_message
{
	unsigned short count;
	volatile unsigned short flags;
	unsigned long what;	
};

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

/**
	@brief ��ȡ��Ϣ

	��Ϣ�����������������⣬һ�㻹����Я������

	@param[in] what ��Ϣ�����û�ʵ���ϲ�ֱ�ӷ�������ṹ�е����ݡ�

	@note:
		�ú������÷�����:
		y_message_read(msg, &arg0, &arg1);
		����arg0 ��argv1 ��unsigned long����
*/
void y_message_read(struct y_message *what, ...);


/************************************************************************/
/* ͬ������                                                                     */
/************************************************************************/
#define Y_SYNC_MAX_OBJS_COUNT 64
#define Y_SYNC_WAIT_INFINITE -1 

typedef enum {
	Y_MSG_LOOP_EXIT_SIGNAL = 0,
	Y_MSG_LOOP_ERROR = -1,
}y_msg_loop_result;

/**
	@brief �ȴ��߳���Ϣ
*/
y_msg_loop_result y_message_loop();

/************************************************************************/
/* �ļ�����                                                                     */
/************************************************************************/
/*
	@brief �ļ��Լ�Ŀ¼�ı��¼�����
*/
typedef enum{
	Y_FILE_EVENT_READ  = (1 << 0),
	Y_FILE_EVENT_WRITE = (1 << 1)
} y_file_event_type_t;

/*
	@brief ���ļ���ʼ���ļ���������
*/
typedef enum{
	Y_FILE_OPERATION_CACHE = 1,
	Y_FILE_OPERATION_NOCACHE
} y_file_operation_type_t;

/**
	@brief ���ļ�

	ϵͳԭ���ӿڣ�Ŀǰ��ľ�о������ƣ���˲���
	��һ��

	@return
		�����ļ�������
*/
y_handle y_file_open(const char *path, y_file_operation_type_t ops_type);

/**
	@brief ��ȡ�ļ�

	��ȡ�򿪳ɹ����ļ���buffer ��

	@param[in] file Ҫ��ȡ���ļ�
	@param[in] buffer Ҫд��Ļ��������û���֤����������������	size���� 	
	@param[in] size ��ȡ����

	@return
		>0 ��ʾ�ɹ���ȡ���ֽ��������ᳬ����size,
		<0��ʾ������.
*/	
ssize_t y_file_read(y_handle file, void *buffer, size_t size);

/**
	@brief д���ļ�

	���û�ָ��buffer д��򿪳ɹ����ļ�

	@param[in] file Ҫд����ļ�
	@param[in] buffer Դ���������û���֤����������������	size���� 	
	@param[in] size д�볤��

	@return
		>0 ��ʾ�ɹ���ȡ���ֽ��������ᳬ����size,
		<0��ʾ������.
*/	
ssize_t y_file_write(y_handle file, void *buffer, size_t size);

/**
	@brief Seek file

*/
int y_file_seek(y_handle file, loff_t where, int whence);

/**
	@brief �ر��ļ�

	�رմ򿪵�y_handle �ļ�
*/
void y_file_close(y_handle file);


/**
	@brief ע���ļ������¼�

	���ļ���ĳ�����������¼����������̴�������ô
	�û�ͨ��������ע��Ļص�����������Ϣѭ����ʱ��
	���ã�����û���Ҫ׼��һ����Ϣѭ�����߳�������
	�ûص���һ���ǵ�һ���߳�������Ϣѭ����

	@param[in] file Ҫ�����ĸ��ļ�
	@param[in] event_mask �¼����ͣ����Զ����ϣ��μ�y_file_event_type_t
	@param[in] func �¼�����ʱ�Ļص�����
	@param[in] para �¼�����ʱ�ص������Ĳ���
	
	@return
		����0������<0 ��Ϊ�����롣
*/
int y_file_event_register(y_handle file, y_file_event_type_t event_mask, void *func, void *para);

/**
	@brief ȡ������ĳ���ļ����¼�

	����û���ǰ������ĳ���ļ���ĳЩ�¼�����ô����ͨ��
	���ӿ�ȡ��������Щ�¼����Ӷ��Ͳ����лص���Ϣ�����͸ñ����̵���Ϣѭ���ϡ�
	���Ҫ��ȡ�����¼���ǰû��ע�ᣬ��ô���������ش����롣

	
	@param[in] file Ҫ�����ĸ��ļ�
	@param[in] event_mask �¼����ͣ����Զ����ϣ��μ�y_file_event_type_t

	@return
		����0������<0 ��Ϊ�����롣
*/
int y_file_event_unregister(y_handle file, y_file_event_type_t event_mask);

/**
	@brief ���ļ�ӳ�����û��ռ������ڴ�
	
	���û�Ҫ��ʹ���ڴ����һ������һ���ļ�,����ʹ�ø÷������ļ�
	ӳ�����û������ڴ�ռ�

	@param[in] file Ҫӳ����ļ�
	@param[in] len  ӳ��ĳ���
	@param[in] prot  ӳ���ҳ��Ȩ��
	@param[in] flags The  flags argument determines whether updates to the mapping are visi-
					ble to other processes mapping the same region, and whether updates are
					carried through to the underlying file.
	@param[in] offset ���ļ�ƫ�Ƴ��ȿ�ʼӳ��

	@return
		�ɹ�����ӳ�䵽�û�����ռ�ĵ�ַ,ʧ�ܷ���NULL
*/
void * y_file_mmap(y_handle file, size_t len, page_prot_t prot, int flags, off_t offset);



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

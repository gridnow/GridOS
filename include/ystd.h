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
#include <kernel/ke_srv.h>

BEGIN_C_DECLS;

/************************************************************************/
/* ����                                                                  */
/************************************************************************/
struct y_thread_environment_block
{
	void *self;
	
	/* Message information */
	void *mi;

	/* Pthread specific key/value */
	void *pt_speci;
};

typedef unsigned long message_id_t;
struct y_message
{
	unsigned short count;
	volatile unsigned short flags;
	message_id_t what;
};
typedef void (*y_message_func)(struct y_message *msg);

#define Y_SYNC_MAX_OBJS_COUNT 64
#define Y_SYNC_WAIT_INFINITE -1
typedef kt_sync_wait_result y_wait_result;
typedef enum {
	Y_MSG_LOOP_EXIT_SIGNAL = 0,
	Y_MSG_LOOP_ERROR = -1,
} y_msg_loop_result;

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
 
	@return
		See y_wait_result.
*/
y_wait_result y_process_wait_exit(y_handle for_who, unsigned long * __in __out result);

/**
	@brief ��ȡ��Ϣ

	��Ϣ�����������������⣬һ�㻹����Я������

	@param[in] what ��Ϣ�����û�ʵ���ϲ�ֱ�ӷ�������ṹ�е�����

	@note:
		�ú������÷�����:
		y_message_read(msg, &arg0, &arg1);
		����arg0 ��argv1 ��unsigned long����
	@return
		The count of parameters the message attached.
*/
int y_message_read(struct y_message *what, ...);

/**
	@brief д�뷵����Ϣ
 
	һ���û�Ϊ��ͬ����Ϣ�ķ���ֵ����Ҫд����Ϣ��ǰ�᱾��д�벻�ܳ����Է�����ʱ��ϢЯ���Ĳ���������
 
	@param[in] what ��Ϣ�����û�ʵ���ϲ�ֱ�ӷ�������ṹ�е�����
	@param[in] wb_count ��д�������������ɶ��ڷ���ʱ������
 
	@return
		The count of parameters written, < 0 for error!
*/
int y_message_writeback(struct y_message *what, int wb_count, ...);

/**
	@brief �ȴ��߳���Ϣ
*/
y_msg_loop_result y_message_loop();

/**
	@brief �ȴ��¼��ķ���
 
	�߳������ȴ��¼��ĳ��������һ���¼��������������������ǻ�û�б���ԭ����ô�ȴ�ʧЧ��ֱ�����±��ָ���
 
	@param[in] event �ȴ����¼�����
	@param[in] timeout �ȴ���ʱ����λΪms��Y_SYNC_WAIT_INFINITE Ϊ���޵ȴ�
 
	@return
		The result in the format of y_wait_result
*/
y_wait_result y_event_wait(y_handle event, int timeout);

/**
	@brief ������Ϣ���߳�
*/
bool y_message_send(ke_handle to_thread, struct y_message *what);

/**
	@brief ע����Ϣ������
*/
bool y_message_register(message_id_t message_id, y_message_func call_back_func);

/**
	@brief ����һ���¼�

	�����¼��ĵȴ���
 
	@param[in] event Ҫ�������¼�
 
	@return
		�����˼����̣߳�< 0��ʾ����
*/
int y_event_set(y_handle event);

/**
	@brief �����¼�����
	
	�����¼�ͬ�����󣬲������¼������͡�
 
	@param[in] manual_reset �Ƿ����Զ���ԭ���¼����������ô��������ỽ��һ���̣߳�������Ϊδ����״̬��������Ҫ�ֶ�����
	@param[in] initial_status �¼�ԭʼ״̬�Ǵ����Ļ���û�б�������
 
	@return
		�ɹ������¼�����ʧ�ܷ���Y_INVALID_HANDLE.
*/
y_handle y_event_create(bool manual_reset, bool initial_status);

/**
	@brief ɾ���¼�
 
	ɾ���¼���������¼����������ڵȴ�����ô��Щ�̶߳��������ѣ�������ȡ�������롣
*/
void y_event_delete(y_handle event);

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

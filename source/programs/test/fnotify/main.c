/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

/**
	
	ϵͳ��������

	file notify
	Wuxin (82828068@qq.com)
 */

#include <stdio.h>
#include <pthread.h>
#include <ystd.h>

#define TEST_FILE "/os/i386/90.nes"
#define EVENT_MASK Y_FILE_EVENT_READ

static void read_event(void *para)
{
	unsigned long file_name;

	y_message_read((struct y_message *)para,&file_name);
	printf("�յ��ļ�%s�Ķ�ȡ�¼���\n", (char *)file_name);
}

static int read_func(void *para)
{
	char buffer[1024];
	ssize_t ret;
	y_handle file = (y_handle)para;

	ret = y_file_read(file, buffer, sizeof(buffer));
	printf("��ȡ�ļ�%s %d�ֽڡ�\n", TEST_FILE, ret);

	return ret;
}

int main()
{
	y_handle file;
	pthread_t read_thread;

	if (Y_INVALID_HANDLE == (file = y_file_open(TEST_FILE, Y_FILE_OPERATION_CACHE)))
		goto err0;

	if (y_file_event_register(file, EVENT_MASK, read_event, TEST_FILE) < 0)
		goto err1;

	/* ����һ���߳�ȥ��ȡ�ļ� �������¼� */
	if (pthread_create(&read_thread, NULL, read_func, file))
		goto err2;

	/* ���� */
	y_message_loop();

	/* ѭ��������ʾ����һ����ֹ */
	y_file_close(file);
	return 0;
	
err2:
	printf("���������߳�ʧ�ܡ�\n");
	return -3;
err1:
	printf("ע������¼�%xʧ�ܡ�\n", EVENT_MASK);
	return -2;
err0:
	printf("Ҫ���Ե��ļ�%s�����ڡ�\n", TEST_FILE);
	return -1;
}

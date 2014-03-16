/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

/**
	
	系统测试用例

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
	printf("收到文件%s的读取事件。\n", (char *)file_name);
}

static int read_func(void *para)
{
	char buffer[1024];
	ssize_t ret;
	y_handle file = (y_handle)para;

	ret = y_file_read(file, sizeof(buffer), buffer);
	printf("读取文件%s %d字节。\n", TEST_FILE, ret);

	return ret;
}

int main()
{
	y_handle file;
	pthread_t read_thread;

	if (Y_INVALID_HANDLE == (file = y_file_open(TEST_FILE)))
		goto err0;

	if (y_file_event_register(file, EVENT_MASK, read_event, TEST_FILE) < 0)
		goto err1;

	/* 创立一个线程去读取文件 ，触发事件 */
	if (pthread_create(&read_thread, NULL, read_func, file))
		goto err2;

	/* 监听 */
	y_message_loop();

	/* 循环出来表示程序一般终止 */
	y_file_close(file);
	return 0;
	
err2:
	printf("创建测试线程失败。\n");
	return -3;
err1:
	printf("注册监听事件%x失败。\n", EVENT_MASK);
	return -2;
err0:
	printf("要测试的文件%s不存在。\n", TEST_FILE);
	return -1;
}

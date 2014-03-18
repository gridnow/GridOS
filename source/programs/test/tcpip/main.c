/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

/**
	
	系统测试用例

	TCPIP
	Wuxin (82828068@qq.com)
 */

#include <stdio.h>
#include <dlfcn.h>
#include <pthread.h>
#include <ystd.h>

#define DEFAULT_WRITE_FILE "/os/net/stream/0"

static pthread_t write_worker;

/*
	ARP Request
	ARP, Ethernet (len 6), IPv4 (len 4), Request who-has 10.137.36.15 tell 10.137.36.21, length 46
*/
unsigned char arp_request[] = {
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd8, 0x9d, 0x67, 0x1a, 0x5c, 0x84, 0x08, 0x06, 0x00, 0x01
        ,0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0xd8, 0x9d, 0x67, 0x1a, 0x5c, 0x84, 0x0a, 0x89, 0x24, 0x15
        ,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x89, 0x24, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        ,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        

/*
	TCP Connect
	TODO:
*/
unsigned char tcp_connect_request[] = {};

static void *write_thread(void *para)
{
	y_handle file;

	if (Y_INVALID_HANDLE == (file = y_file_open(DEFAULT_WRITE_FILE)))
		goto err0;
	
	while(1)
	{		
		y_file_write(file, arp_request, sizeof(arp_request));
		y_file_seek(file, 0, SEEK_SET);
	}

	y_file_close(file);
	return NULL;

err0:
	printf("输入文件不存在。\n");
	return NULL;
}

int main()
{
	void *so = dlopen("tcpip.so", 0);	
	int (*entry)(int argc, char **argv);
	
	if (!so)
	{
		printf("协议栈动态库加载失败。\n");
		goto err;
	}
	
	entry = dlentry(so);
	if (!entry)
	{
		printf("Cannot get the entry address of this shared object.\n");
		goto err;
	}
	entry(0, NULL);

	/* 写包线程 */	
	if (pthread_create(&write_worker, NULL, write_thread, 0))
		goto err;
	
	return 0;
err:
	return -1;	
}

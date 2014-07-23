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
#include <stdlib.h>
#include <string.h>
#include <socket.h>

#include "ring_buff.h"

#include <ddk/net.h>

#define DEFAULT_WRITE_FILE "/os/net/stream/0"

static pthread_t write_worker;

#define CACHE_LEN (4 * 1024)
#define max_test (256 * 1024)
static char test[max_test];
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
unsigned char tcp_connect_request[] = {
		0x48,0x46,0xFB,0xD7,0x6A,0x17,0xC8,0x9C,0xDC,0xFE,0xA8,0x3B,0x08,0x00,0x45,0x00,0x00,0x34,0x27,	0x4E,0x40,
		0x00,0x40,0x06,0xB6,0x3D,0x0A,0x89,0x24,0x18,0x0A,0x89,0x24,0x0F,0x04,0x99,0x00,0x50,0xEB,0xB0,0xEE,0xDF,
		0x00,0x00,0x00,0x00,0x80,0x02,0xFF,0xFF,0x32,0x63,0x00,0x00,0x02,0x04,0x05,0xB4,0x01,0x03,0x03,0x03,0x01,
		0x01,0x04,0x02};

/*
	UDP packet
	TODO:
*/
unsigned char udp_data[] = {
		0xC8,0x9C,0xDC,0xFE,0xA8,0x3B,0x48,0x46,0xFB,0xD7,0x6A,0x17,0x08,0x00,0x45,0x00,0x00,0x7B,0xE3,	0xBA,0x00,0x00,0x3F,
		0x11,0x3A,0x71,0x0A,0x89,0x24,0x26,0x0A,0x89,0x24,0x0F,0x09,0x79,0x09,0x79,0x00,0x67,0x49,0x52,0x31,0x5F,0x6C,0x62,
		0x74,0x34,0x5F,0x30,0x23,0x33,0x38,0x34,0x23,0x43,0x38,0x39,0x43,0x44,0x43,0x46,0x45,0x41,0x34,0x42,0x32,0x23,0x30,
		0x23,0x30,0x23,0x30,0x23,0x32,0x2E,0x35,0x61,0x3A,0x31,0x33,0x39,0x35,0x34,0x36,0x38,0x37,0x37,0x34,0x3A,0x44,0x61,
		0x79,0x73,0x74,0x61,0x72,0x74,0x3A,0x44,0x41,0x59,0x53,0x54,0x41,0x52,0x54,0x3A,0x36,0x32,0x39,0x31,0x34,0x35,0x37,
		0x3A,0xB8,0xDF,0xBA,0xA3,0xCB,0xC9,0x2D,0x58,0x50,0x00,0xCE,0xDE,0xCF,0xDF,0xCD,0xF8,0xC2,0xE7,0xB2,0xBF,0x00};


static void *write_thread(void *para)
{
	y_handle file;
	int loop_times = 2;
	void *map_start = NULL;
	unsigned char *arp_p = NULL, *tcp_p = NULL, *udp_p = NULL;
	struct ring_buff_cache * cache;
	
	if (Y_INVALID_HANDLE == (file = y_file_open(DEFAULT_WRITE_FILE, Y_FILE_OPERATION_NOCACHE)))
		goto err0;
	
	/* mmap */
	map_start = y_file_mmap(file, 0, KM_PROT_READ | KM_PROT_WRITE, 0, 0);
	if (!map_start)
		goto err1;

	memset(map_start, 0, CACHE_LEN);

	/* init ring buff */
	cache = ring_buff_head_init(map_start, CACHE_LEN);
	
	cache_package_head_info_debug(cache);
	
	while (loop_times--)
	{
		/* alloc arp */
		arp_p = ring_buff_alloc(cache, sizeof(arp_request));
		if (!arp_p)
			goto err0;
		memcpy(arp_p, arp_request, sizeof(arp_request));

		tcp_p = ring_buff_alloc(cache, sizeof(tcp_connect_request));
		if (!tcp_p)
			goto err0;
		memcpy(tcp_p, tcp_connect_request, sizeof(tcp_connect_request));
	
		udp_p = ring_buff_alloc(cache, sizeof(udp_data));
		if (!udp_p)
			goto err0;
		memcpy(udp_p, udp_data, sizeof(udp_data));
	}
	
	/* DEBUG 显示cache分配情况 */
	cache_package_head_info_debug(cache);

	/* 触发协议栈收报 */
	y_file_read(file, &test, sizeof(test));
	
err1:
	y_file_close(file);
	return NULL;

err0:
	printf("输入文件不存在。\n");
	return NULL;
}

static int acccept_thread_counts = 0;
static int socket_thread_counts  = 0;

static void *socket_thread(void *unused)
{
	struct sockaddr addr;
	struct sockaddr_in *addr_inet = (struct sockaddr_in *)&addr;
	socklen_t addrlen;
	int r, fd;
	char buf[1000] = {0};
	
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd== -1)
	{
		printf("Create socket error.\n");
	}
	printf("Socket id is %d.\n", fd);
	
	printf("Connecting....");
	memset(&addr, 0, sizeof(addr));
	addrlen = sizeof(addr);

	addr_inet->sin_len = addrlen;
	addr_inet->sin_family = AF_INET;
	addr_inet->sin_port   = 0x123;
	addr_inet->sin_addr.s_addr = 0x0f24890a;
	
	r = connect(fd, &addr, addrlen);
	printf("result is %d.\n", r);

	while (1)
	{

		r = recv(fd, (void *)buf, sizeof(buf), 0x01);
		if (r != sizeof(buf))
		{
			printf("链接线程 recv ret %d.\n", r);
			break;
		}
		//printf("链接线程 接收到报文 %d 字节\n", r);
		//printf("%x, %x, %x,%x, %x, %x,%x, %x, %x, %x.\n", buf[0], buf[1],buf[2],buf[3],buf[4],buf[5],
		//		buf[6],buf[7],buf[8],buf[9]);
	
		memset(buf, 0x23, sizeof(buf));
		r = send(fd, (void *)buf, sizeof(buf), 0x01);
		if (r != sizeof(buf))
		{
			printf("链接线程 send ret %d.\n", r);
			break;
		}
		//printf("链接线程 发送字节 %d\n", r);
		socket_thread_counts++;
	}
	
	//TODO: close socket
	
	return NULL;
}


/**
	@brief accept thread for tcp
	
*/
static void *accept_thread(void *para)
{
	struct sockaddr addr;
	struct sockaddr_in *addr_inet = (struct sockaddr_in *)&addr;
	socklen_t addrlen;
	int fd, newfd, ret;
	char buf[1000] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,};
	
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
	{
		printf("Create socket error.\n");
	}
	printf("socket id is %d.\n", fd);
	
	printf("Binding....");
	memset(&addr, 0, sizeof(addr));
	addrlen = sizeof(addr);

	addr_inet->sin_len = addrlen;
	addr_inet->sin_family = AF_INET;
	addr_inet->sin_port   = 291;
	addr_inet->sin_addr.s_addr = 0x0f24890a;
	
	ret = bind(fd, (const struct sockaddr *)&addr, addrlen);
	printf("bind result %d\n", ret);

	ret = listen(fd, 12);
	printf("listen result %d\n", ret);
	
	newfd = accept(fd, &addr, &addrlen);
	printf("接受 result newfd %d\n", newfd);

	while (1)
	{

		ret = send(newfd, (void *)buf, sizeof(buf), 0x01);
		if (ret != sizeof(buf))
		{
			printf("接收线程 send ret %d.\n", ret);
			break;
		}
		//printf("接收线程 发送字节 %d\n", ret);
#if 1
		ret = recv(newfd, (void *)buf, sizeof(buf), 0x01);
		if (ret != sizeof(buf))
		{
			printf("接收线程 recv ret %d.\n", ret);
			break;
		}
#endif
		
		//printf("接收线程 接收到报文 %d 字节\n", ret);
		//printf("接收线程 %x, %x, %x,%x, %x, %x,%x, %x, %x, %x.\n", buf[0], buf[1],buf[2],buf[3],buf[4],buf[5],
		//		buf[6],buf[7],buf[8],buf[9]);

		memset(buf, 0x45, sizeof(buf));
		acccept_thread_counts++;
	}
	
	return NULL;
}

int main()
{
	void *so = dlopen("tcpip.so", 0);	
	int (*entry)(int argc, char **argv);
	y_handle hevent;
	int accept_counts =0, socket_counts = 0;
	
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
	
	/* Wait 1ms for tcpip to startup */
	hevent = y_event_create(false, false);
	y_event_wait(hevent, 1000);
	printf("OK, let's go test tcpip.\n");

	/* 写包线程 */	
	//if (pthread_create(&write_worker, NULL, write_thread, 0))
	//	goto err;

	/* test bind thread */
	if (pthread_create(NULL, NULL, accept_thread, 0))
		goto err;
	
	/* Socket testing thread */
	if (pthread_create(NULL, NULL, socket_thread, 0))
		goto err;
	
	/* We cannot exit , so wait */
	while (1)
	{
		/* 记录等待前计数 */
		accept_counts = acccept_thread_counts;
		socket_counts = socket_thread_counts;
		
		/* waits 1s */
		y_event_wait(hevent, 1000);
		printf("accept thread counts %d,socket thread counts %d.\n",
				acccept_thread_counts - accept_counts, socket_thread_counts - socket_counts);
		if ((socket_thread_counts - socket_counts) == 0)
		{
			break;
		}
			
	}
	y_event_wait(hevent, 1000000000);
	return 0;
err:
	return -1;	
}

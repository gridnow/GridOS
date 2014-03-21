/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

/**
	
	系统测试用例

	Grid模型测试，在不修改或者很少代码修改的情况下实现Grid。
 
	多线程pi 算法，论文参见http://wenku.baidu.com/link?url=BzcD4EillwKzqRph2Kkzh5nUT4xbLen7eEC5mjRwhUcAJwdt_RYY2L7aK6iTuKvhccV24yLlB4q74Te65TK4xZltngrhFUSNHj_bbMAEJf3
	Wuxin (82828068@qq.com)
 */

#include <stdio.h>
#include <pthread.h>
#include <ystd.h>

#define N 100000000
#define NUM 4

static double sum = 0.0;

static void *worker_func(void *para)
{
	unsigned long j;
	double tempsum, x, partial = 0;

	for (j = (unsigned long)para; j < N; j += NUM)
	{
        x = (j + 0.5f ) / N;
        partial += 4.0f / (1.0f + x * x);
	}
	tempsum = partial * (1.0 / N);

//	pthread_mutex_lock(&mutex);
	sum += tempsum;
//	pthread_mutex_unlock(&mutex);
	return NULL;
}

int main()
{
	int i;
	pthread_t work_thread;
	
	for (i = 0; i < NUM; i++)
	{
		if (pthread_create(&work_thread, NULL, worker_func, (void*)i))
			goto err1;
	}
	

	/* 监听 */
	y_message_loop();

	return 0;
	
err1:
	printf("无法创立工作者线程运算。\n");
	return -2;
}

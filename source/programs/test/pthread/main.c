/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

/**
	pthread function test
*/

#include <pthread.h>
#include <stdlib.h>
#include <ystd.h>

static void *get_thread_specific(void *para)
{
	int *a = NULL, b = 18;
	int result = 0;
	pthread_key_t key;
	
	/* pthread key create */
	while (b)
	{
		result = pthread_key_create(&key, NULL);
		if (result < 0)
			goto err;
		b--;
		printf("create key is %d.\n", key);
	}

	b = 10;
	/* set value */
	result = pthread_setspecific(key, &b);
	if (result < 0)
		goto err;

	/* delete the key */
	result = pthread_key_delete(key);
	if (result < 0)
		goto err;
	
	/* get value base on key */
	a = (int *)pthread_getspecific(key);
	if (!a)
	{
		printf("pthread getspecific failt.\n");
		result = pthread_key_create(&key, NULL);
		if (result < 0)
			goto err;
		printf("create key is %d.\n", key);
		goto err;
	}

	printf("the specific key %d value %d", key, *a);
	
err:
	printf("result %d\n", result);
	return NULL;
}

int main(int argc, char **agrv)
{
	/* create wait event */
	y_handle wait_event = y_event_create(false, false);

	void *p = malloc(12);
	printf("start pthread set value test p %p.\n", p);

	
	/* start pthread */
	if (pthread_create(NULL, NULL, get_thread_specific, NULL) < 0)
	{
		printf("pthread create failt.\n");
		return 0;
	}

	/* wait 5s */
	while (1)
		y_event_wait(wait_event, 50000);
	return 0;
}





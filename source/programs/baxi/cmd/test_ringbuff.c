
/**
	test for ring_buff
*/

#include <ring_buff.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmd.h"

static int test_ring(int argc, char **argv)
{
	
	char *p = NULL, *p1 = NULL, *p2 = NULL,*p3 = NULL;
	
	struct ring_buff_cache *buff = malloc(1024);
	if (!buff)
		goto err;
	
	buff = ring_buff_head_init(buff, 1024);
	
	printf("Allocating 3 buffers\n");
	p = ring_buff_alloc(buff, 256);
	if (!p)
		goto err;
	memset((void *)p, 1, 256);
	
	p1 = ring_buff_alloc(buff, 300);
	if (!p1)
		goto err;
	memset((void *)p1, 2, 300);
	
	p2 = ring_buff_alloc(buff, 123);
	if (!p2)
		goto err;
	memset((void *)p2, 3, 123);
	
	printf("Free the first one\n");
	ring_buff_free(buff, p);
	
	printf("Allocate again the size of first one, the address should meet\n");
	p3 = ring_buff_alloc(buff, 256);
	if (p != p3)
	{
		printf("Error in first round test p = %p, p3 = %p\n", p, p3);
		goto err;
	}
	memset((void *)p3, 4, 256);
	
	printf("Free the second one\n");
	ring_buff_free(buff, p1);
	
	printf("Allocating bigger the 2nd one\n");
	p = ring_buff_alloc(buff, 301);
	if (p == p1)
		printf("Error in p1\n");
	printf("2nd p = %p, p1 = %p\n", p, p1);

	ring_buff_free(buff, p2);
	ring_buff_free(buff, p3);
	p2 = NULL;
	p3 = NULL;
	
	/* Show status */
	cache_package_head_info_debug(buff);
err:
	if (buff)
		free(buff);
	
	return 0;
}


struct cmd CMD_ENTRY cmd_test_ringbuff = {
	.name = "test-ringbuff",
	.desc = 0,
	.help = 0,
	.func = test_ring,
};




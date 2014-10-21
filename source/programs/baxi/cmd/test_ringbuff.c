
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
	
	buff = ring_buff_head_init(buff, 2048);
	
	p = ring_buff_alloc(buff, 256);
	if (!p)
		goto err;
	memset((void *)p, 1, 256);
	
	p1 = ring_buff_alloc(buff, 300);
	if (!p1)
		goto err;
	memset((void *)p1, 3, 300);	
	
	p2 = ring_buff_alloc(buff, 123);
	if (!p2)
		goto err;
	memset((void *)p2, 1, 123);	
	
	ring_buff_free(buff, p);
	p = NULL;

	/* DEBUG FOR CURR HEAD INFO */
	cache_package_head_info_debug(buff);
	
	p3 = ring_buff_alloc(buff, 256);
	
	ring_buff_free(buff, p1);
	p1 = NULL;
	
	if (!p3)
		goto err;
	memset((void *)p3, 1, 256);	

	cache_package_head_info_debug(buff);
	
	ring_buff_free(buff, p2);
	ring_buff_free(buff, p3);	
	p2 = NULL;
	p3 = NULL;
	
	cache_package_head_info_debug(buff);
err:
	cache_package_head_info_debug(buff);
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




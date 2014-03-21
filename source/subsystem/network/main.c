/**
	Network framework

	Sihai
*/
#include <types.h>
#include <stdio.h>
#include <compiler.h>
#include <ddk/net.h>

struct nss_hwmgr *hwmgr;
static void *nss_dev = (void*)"eth0";

static void dump_raw_package(void *buf, int size)
{
	unsigned char *p = buf;
	int off = 0;

	while (size > 0)
	{		
		if ((off % 16) == 0)
			printf("\n%08x: ", off);

		printf("%02x ", p[off]);
		off++;
		size--;
	}
}

static int input_stream(void *nss_dev, void *data, size_t size)
{
	//printf("网卡%s收到物理包，字节%d.\n", nss_dev, size);
	//dump_raw_package(data, size);
	return 0;
}

/************************************************************************/
/* Controller                                                           */
/************************************************************************/
static void probe_avaliber_netif()
{
	
}


/* Called now by DSS */
DLLEXPORT void nss_hwmgr_register(struct nss_hwmgr *mgr)
{
	hwmgr = mgr;

	printf("启动网络设备...\n");

	hwmgr->nops->read = input_stream;
	hwmgr->nops->open("eth0", nss_dev);
}

void nss_main()
{
	hwmgr = NULL;
}

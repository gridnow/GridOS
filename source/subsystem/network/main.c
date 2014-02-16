/**
	Network framework

	Sihai
*/
#include <types.h>
#include <compiler.h>
#include <ddk/net.h>

struct nss_hwmgr *hwmgr;

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

//	hwmgr->nops->open("eth0");
}

void nss_main()
{
	hwmgr = NULL;
}

/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *
 *   网络子系统之硬件管理器
 */
#ifndef DDK_NSS_HWMGR_H
#define DDK_NSS_HWMGR_H

#define NSS_HWMGR_VERSION_CUR 0


/* 由于我们现在还没有完备的设备管理器，因此先抽象一套网卡设备管理器的接口处理 */
struct nss_netif_ops
{
	int (*open)(const char *name, void *nss_device);
	int (*close)(const char *name);

	int (*write)();

	/* Read is hooked by upper layer */
	int (*read)(void *nss_device, void *data, size_t len);
};

struct nss_hwmgr
{
	char *name;
	int version;
	struct nss_netif_ops *nops;	
};

extern void nss_hwmgr_register(struct nss_hwmgr *mgr);

#endif
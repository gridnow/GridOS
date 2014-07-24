/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *
 *   ������ϵͳ֮Ӳ��������
 */
#ifndef DDK_NSS_HWMGR_H
#define DDK_NSS_HWMGR_H

#define NSS_HWMGR_VERSION_CUR 0


/* �����������ڻ�û���걸���豸������������ȳ���һ�������豸�������Ľӿڴ��� */
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
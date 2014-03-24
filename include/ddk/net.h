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

struct package_pos
{
	/* 包大小 */
	size_t package_size;
	/* 数据位置 */
	unsigned long package_offset;
};

struct package_head
{
	/* 包头包含的数据包数目 */
	size_t package_count;
	/* 下一个包头的位置 */
	unsigned long next_head_offset;
	struct package_pos pos[0];
};

#define DATA_SIZE(BYTE) (((BYTE) + sizeof(struct package_head) - 1)/sizeof(struct package_head))

/* 该宏暂时能用于一个报文的封装 TODO */
#define MAKE_PACKAGE(head, nbyte) do{\
	(head)->package_count = 1;\
	(head)->next_head_offset = (nbyte) + sizeof(struct package_head) + sizeof(struct package_pos);\
	(head)->pos->package_size = (nbyte);\
	(head)->pos->package_offset = sizeof(struct package_head) + sizeof(struct package_pos);\
	}while(0)

/* 获取next package head offset */
#define get_package_next_head(head) \
	(((struct package_head *)(head))->next_head_offset)

/* 获取package pos */
#define get_package_pos(head) \
	(((struct package_head *)(head))->pos->package_offset)

/* 获取package size */
#define get_package_size(head) \
	(((struct package_head *)(head))->pos->package_size)


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
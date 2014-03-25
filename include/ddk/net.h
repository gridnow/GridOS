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

struct package_pos
{
	/* ����С */
	size_t package_size;
	/* ����λ�� */
	unsigned long package_offset;
};

struct package_head
{
	/* ��ͷ���������ݰ���Ŀ */
	size_t package_count;
	/* ��һ����ͷ��λ�� */
	unsigned long next_head_offset;
	struct package_pos pos[0];
};

#define DATA_SIZE(BYTE) (((BYTE) + sizeof(struct package_head) - 1)/sizeof(struct package_head))

/* �ú���ʱ������һ�����ĵķ�װ TODO */
#define MAKE_PACKAGE(head, nbyte) do{\
	(head)->package_count = 1;\
	(head)->next_head_offset = (nbyte) + sizeof(struct package_head) + sizeof(struct package_pos);\
	(head)->pos->package_size = (nbyte);\
	(head)->pos->package_offset = sizeof(struct package_head) + sizeof(struct package_pos);\
	}while(0)

/* ��ȡnext package head offset */
#define get_package_next_head(head) \
	(((struct package_head *)(head))->next_head_offset)

/* ��ȡpackage pos */
#define get_package_pos(head) \
	(((struct package_head *)(head))->pos->package_offset)

/* ��ȡpackage size */
#define get_package_size(head) \
	(((struct package_head *)(head))->pos->package_size)


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
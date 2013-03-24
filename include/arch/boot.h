/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

/**
 * @defgroup Arch
 * @ingroup Archƽ̨��ض���
 *
 * ϵͳ����ʱ�ṩ��������Ϣ
 *
 * @{
 */
#ifndef	__INITLOADER_H__
#define	__INITLOADER_H__

#ifdef	__i386__
#define	STARTUP_ARGS_ADDR   0x100000UL
#define STARTUP_ARGS_LEN    0x10000UL
/* TODO Add other arch */
#else
#error "Unsupport arch!"
#endif

#define ARCH_SO_NAME "arch.so"

#define STARTUP_MEMORY_AVAILABLE    1   /**<    �ÿ��ڴ��Ǳ�����*/
#define STARTUP_MEMORY_RESERVED     2   /**<    �ÿ��ڴ��ǿ��õ�*/


/* �����ڴ�ӳ��� */
struct startup_memory_map
{
    u32	start_addr_lowpart;             /**<    �ڴ���׵�ַ,��32λ    */
    u32 start_addr_highpart;            /**<    �ڴ���׵�ַ,��32λ    */
    u32	length_lowpart;                 /**<    �ڴ��ĳ���,��32λ    */
    u32 length_highpart;                /**<    �ڴ��ĳ���,��32λ     */
    u32	type;                           /**<    �ڴ������� */
};

/* ������Ƶ��Ϣ */
struct starup_video_info
{
    u16 bpp;            /**<    ÿ�����ص�λ�� */
    
    u16 res_width;      /**<    ˮƽ�ֱ���  */
    u16 res_height;     /**<    ��ֱ�ֱ���  */
#ifdef  __i386__
    u32 fb_addr;        /**<    Frame Buffer����ַ*/
#else
    /* TODO: �������ƽ̨��֧�������� */
#endif
};

/* �����豸��Ϣ */
struct startup_boot_dev_info
{
#ifdef  __i386__
    int boot_dev;   /**<    ��x86�Ĵ�ͳBios����ģʽ�£�int 13h�������ƶ���ʹ�õĴ��̱�� */
#endif
};

/* ģ����Ϣ */
struct startup_modules
{
    u32 mod_name;   /**<    ģ������,ASCIZ��ʽ   */
    u32 mod_addr;   /**<    ģ��Ŀ�ʼ��ַ  */
    u32 mod_length; /**<    ģ��ĳ��� */
};

/* �������� */
struct startup_args
{
#ifdef  __i386__
    u32         low_mem;    /**<    �Ͷ��ڴ� */
    u32         high_mem;   /**<    �߶��ڴ�  */
#endif
    /* �ڴ沼���б� */
    u32         nr_mem_map; /**<    �ڴ��б��ж��ٽڵ�    */
    u32         mem_map;    /**<    ָ���ڴ�ӳ��ڵ����׵�ַ��startup_memory_map����    */
    
    /* ����ʱ��ʾ���� */
    u32         vid_info;   /**<    ��ʾ��Ϣ���׵�ַ    */
    
    /* �����豸���� */
    u32         boot_dev;   /**<    �����豸���׵�ַ    */
    
    /* ����ģ���б� */
    u32         nr_mods;    /**<    ģ��ĸ���  */
    u32         mods;       /**<    ģ���б���׵�ַ */
    
    /* �ں������������в��� */
    u32         knl_cmd_line_length;    /**< �ں����������еĳ���  */
    u32         knl_cmd_line;           /**< �ں������е��׵�ַ��ASCIZ���� */
};

#endif

/** @}*/

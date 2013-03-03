/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

/**
 * @defgroup Arch
 * @ingroup Arch平台相关定义
 *
 * 系统启动时提供的启动信息
 *
 * @{
 */
#ifndef	__INITLOADER_H__
#define	__INITLOADER_H__

#ifdef	__i386__
#define	STARTUP_ARGS_ADDR 0x90000
/* TODO Add other arch */
#else
#error "Unsupport arch!"
#endif

#define STARTUP_MEMORY_AVAILABLE    1   /**<    该块内存是保留的*/
#define STARTUP_MEMORY_RESERVED     2   /**<    该块内存是可用的*/


/* 启动内存映射表 */
struct startup_memory_map
{
    u64	start_address;  /**<    内存块首地址    */
    u64	length;         /**<    内存快的长度    */
    u32	type;           /**<    内存快的类型 */
};

/* 启动视频信息 */
struct starup_video_info
{
    u16 bpp;            /**<    每个像素的位数 */
    
    u16 res_width;      /**<    水平分辨率  */
    u16 res_height;     /**<    垂直分辨率  */
#ifdef  __i386__
    u32 fb_addr;        /**<    Frame Buffer基地址*/
#else
    /* TODO: 添加其他平台的支持在这里 */
#endif
};

/* 启动设备信息 */
struct startup_boot_dev_info
{
#ifdef  __i386__
    int boot_dev;   /**<    在x86的传统Bios启动模式下，int 13h服务所制定和使用的磁盘编号 */
#endif
};

/* 模块信息 */
struct startup_modules
{
    u32 mod_name;   /**<    模块名称,ASCIZ格式   */
    u32 mod_addr;   /**<    模块的开始地址  */
    u32 mod_length; /**<    模块的长度 */
};

/* 启动参数 */
struct startup_args
{
#ifdef  __i386__
    u32         low_mem;    /**<    低端内存 */
    u32         high_mem;   /**<    高端内存  */
#endif
    /* 内存布局列表 */
    u32         nr_mem_map; /**<    内存列表共有多少节点    */
    u32         mem_map;    /**<    指向内存映射节点表的首地址，startup_memory_map类型    */
    
    /* 启动时显示参数 */
    u32         vid_info;   /**<    显示信息的首地址    */
    
    /* 启动设备参数 */
    u32         boot_dev;   /**<    引导设备的首地址    */
    
    /* 启动模块列表 */
    u32         nr_mods;    /**<    模块的个数  */
    u32         mods;       /**<    模块列表的首地址 */
    
    /* 内核启动的命令行参数 */
    u32         knl_cmd_line_length;    /**< 内核启动命令行的长度  */
    u32         knl_cmd_line;           /**< 内核命令行的首地址，ASCIZ类型 */
};

#endif

/** @}*/

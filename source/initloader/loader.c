/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

#include <types.h>
#include <stddef.h>

#include <arch/boot.h>

#include "utils.h"
#include "elf.h"

#ifdef __i386__
#include "grub.h"

/* Check if the bit BIT in FLAGS is set.  */
#define CHECK_FLAG(flags,bit)	((flags) & (1 << (bit)))
#define MAX_FLAGS               12
typedef void (*fill_args_func)(struct startup_args *args, multiboot_info_t *mbi);


static void mem_args(struct startup_args *args, multiboot_info_t *mbi)
{
    args->low_mem   = mbi->mem_lower;
    args->high_mem  = mbi->mem_upper;
}

static void bootdev_args(struct startup_args *args, multiboot_info_t *mbi)
{
    struct startup_boot_dev_info *bootdev_info;
    
    bootdev_info = alloc_startup_args_mem(sizeof(struct startup_boot_dev_info));
    bootdev_info->boot_dev = mbi->boot_device;
    
    args->boot_dev = (u32)bootdev_info;
}

static void cmd_line_args(struct startup_args *args, multiboot_info_t *mbi)
{
    args->knl_cmd_line_length   = __strlen((const char*)mbi->cmdline) + 1;
    args->knl_cmd_line          = (u32)alloc_startup_args_mem(args->knl_cmd_line_length);
    __strcpy((char *)args->knl_cmd_line, (char *)mbi->cmdline);
}

static void mods_args(struct startup_args *args, multiboot_info_t *mbi)
{
    struct startup_modules *mods;
    module_t *grub_mods;
    
    unsigned long i;
    
    mods = alloc_startup_args_mem(sizeof(struct startup_modules) * mbi->mods_count);
    grub_mods = (module_t *)mbi->mods_addr;
    
    for (i = 0; i < mbi->mods_count; i++)
    {
        size_t name_len = __strlen((const char *)grub_mods[i].string);
        mods[i].mod_name    = (u32)alloc_startup_args_mem(name_len + 1);
        __strcpy((char *)mods[i].mod_name, (char *)grub_mods[i].string);
        mods[i].mod_length  = grub_mods[i].mod_end - grub_mods[i].mod_start;
        mods[i].mod_addr    = grub_mods[i].mod_start;
    }
    
    args->nr_mods   = mbi->mods_count;
    args->mem_map   = (u32)mods;
}

static void void_args(struct startup_args *args, multiboot_info_t *mbi)
{
    /* shit!!! haha */
    args = args;
    mbi = mbi;
}

static void mmap_args(struct startup_args *args, multiboot_info_t *mbi)
{
    memory_map_t *grub_map;
    struct startup_memory_map *mmap;
    
    unsigned long i;
    
    mmap = (struct startup_memory_map*)alloc_startup_args_mem(
            sizeof(struct startup_memory_map) * mbi->mmap_length
            );
    
    grub_map = (memory_map_t *)mbi->mmap_addr;
    for (i = 0; i < mbi->mmap_length; i++)
    {
        mmap[i].length_lowpart      = grub_map[i].length_low;
        mmap[i].length_highpart     = grub_map[i].length_high;
        mmap[i].start_addr_lowpart  = grub_map[i].base_addr_low;
        mmap[i].start_addr_highpart = grub_map[i].base_addr_high;
        
        mmap[i].type    = grub_map[i].type;
    }
    
    
    args->nr_mem_map= mbi->mmap_length;
    args->mem_map   = (u32)mmap;
}

static void vbe_args(struct startup_args *args, multiboot_info_t *mbi)
{
    /* TODO: Add code here */
}

static fill_args_func fill_args_ops[] = {
    mem_args,       /* bit  0: memory lower and memory high */
    bootdev_args,   /* bit  1: boot device number, int13h service */
    cmd_line_args,  /* bit  2: command line to kernal */
    mods_args,      /* bit  3: core modules list  */
    void_args,      /* bit  4: a.out info, ignore    */
    void_args,      /* bit  5: elf info, ignore   */
    mmap_args,      /* bit  6: memory map list   */
    void_args,      /* bit  7: drivers info, ignore  */
    void_args,      /* bit  8: biso config table, ignore   */
    void_args,      /* bit  9: boot loader name, ignore   */
    void_args,      /* bit 10: apm table, ignore   */
    void_args       /* bit 11: vbe information  */
};

void init_main(unsigned long magic, unsigned long addr)
{
    multiboot_info_t *mbi;
    struct startup_args *args;
    struct startup_modules *mods;
    struct startup_modules *mod_arch;
    u32 i;
    int flag;
    void (*arch_entry)(void);
    
    cls();
    
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        /* not a multiboot loader, invalid boot magic */
        printf("Invalid boot magic\n");
        return;
    }
    
    mbi = (multiboot_info_t*)addr;
    
    args = alloc_startup_args_mem(sizeof(struct startup_args));
    for (flag = 0; flag < MAX_FLAGS; flag++)
    {
        fill_args_ops[flag](args, mbi);
    }
    
    /* search arch module */
    mods = (struct startup_modules *)args->mods;
    mod_arch = NULL;
    for (i = 0; i < args->nr_mods; i++)
    {
        if (!__strcmp((char *)mods[i].mod_name, ARCH_SO_NAME))
        {
            mod_arch = &mods[i];
            break;
        }
    }
    
    if (!mod_arch)
    {
        printf("The " ARCH_SO_NAME " was not loaded!\n");
        return;
    }
    
    printf("Grid OS will start\n");
    arch_entry = (void (*)())load_elf_from_mem((void *)mod_arch->mod_addr);
    
    arch_entry();
}

#endif /* __i386__ */
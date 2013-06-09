
#include <hal_config.h>

#include <asm/abicall.h>

/*
	The load structure should be same with the loader.
	The package should be located at lower address while kernel at higher address,
	so the kernel can protect the package space from been hurting.
*/
#define KE_BOOTMOD_SEARCH_MAGIC 0x4b42534d /*KBSM*/
#ifdef __mips__
#define KE_BOOTMOD_SEARCH_START 0x80200000
#define KE_BOOTMOD_SEARCH_SIZE	(10 * 1024 * 1024)
#define KE_BOOTMOD_SEARCH_GRAN	( 1 * 1024 * 1024)
#endif
#ifdef __i386__
#define KE_BOOTMOD_SEARCH_START HAL_GET_BASIC_KADDRESS(0x200000)
#define KE_BOOTMOD_SEARCH_SIZE	(10 * 1024 * 1024)
#define KE_BOOTMOD_SEARCH_GRAN	( 1 * 1024 * 1024)
#endif
#ifndef KE_BOOTMOD_SEARCH_START
#define KE_BOOTMOD_SEARCH_START	0
#define KE_BOOTMOD_SEARCH_SIZE	0
#define KE_BOOTMOD_SEARCH_GRAN	0
#endif

struct kernel_loaded_module_entry
{
	unsigned int name_offset;
	unsigned int file_offset;
	unsigned int file_size;
};
struct kernel_loaded_module_head
{
	unsigned int magic;
	unsigned int version;
	unsigned int module_count;
	struct kernel_loaded_module_entry entry[];
};
static struct kernel_loaded_module_head * head;
/**
	@brief find the loaded module table. Different platform has different position
*/
static long search_head()
{
	int * p = (int*)(int)KE_BOOTMOD_SEARCH_START;
	int size = KE_BOOTMOD_SEARCH_SIZE;
	
	for (size = 0; size < KE_BOOTMOD_SEARCH_SIZE; size += KE_BOOTMOD_SEARCH_GRAN)
	{
		if (*p == KE_BOOTMOD_SEARCH_MAGIC)
			return (long)p;
		p = (int*)((unsigned long)p + KE_BOOTMOD_SEARCH_GRAN);
#if 0
		/* ELF头去不掉的情况 */
		{
			int i = 0x100;							//最大检查的范围
			int g = 0x10;							//增量
			int s = 0xe0;							//start search offset

			int j = 0;

			/* Move to the start offset */
			p = (int *)((unsigned long)p + s); 
			while (j < i)
			{		
				if (*p == KE_BOOTMOD_SEARCH_MAGIC)
					return (long)p;
				j += g;
				p = (int *)(unsigned long)p + g; 				
			}
		}
#endif
	}

	return 0;
}

/**
	@brief get the head
*/
static struct kernel_loaded_module_head * get_head()
{
	if (head) return head;
	head = (struct kernel_loaded_module_head*)search_head();
	return head;
}


/**
	@brief loop the loaded file to get total size
*/
int hal_boot_module_size()
{
	int i;
	int total = 0;
	struct kernel_loaded_module_head * head = get_head();
	if (!head) goto end;
	for (i = 0; i < head->module_count; i++)
	{
		total += head->entry[i].file_size;
	}
end:
	return total;
}


/**
	@brief loop the loaded file
*/
void hal_boot_module_loop(void * (*loop)(void *data, int size, char *name, int id))
{
	int i;
	struct kernel_loaded_module_head * head = get_head();
	if (!head)
		return;
	
	//printk("loop loaded module at %p...\n", head); 
	
	for (i = 0; i < head->module_count; i++)
	{	
		ka_call_dynamic_module_entry(loop,
			(void*)(head->entry[i].file_offset + (unsigned long)head),
			head->entry[i].file_size,
			(char*)(head->entry[i].name_offset + (unsigned long)head),
			i);
	}
}


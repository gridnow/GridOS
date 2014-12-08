#define SET_LAZY 1

#include "../../../programs/dl/main.c"

#include <unistd.h>
#include <stdio.h>

/******************************************************
 For CRT
 ******************************************************/
#define LIBC_START_MAIN __libc_start_main

DLLEXPORT __attribute__ ((noreturn))  int LIBC_START_MAIN (int (*main) (int, char **, char **),
							int argc,
							char **argv,
							__typeof (main) init,
							void (*fini) (void),
							void (*rtld_fini) (void),
							void *stack_end)
{
	int r;
TODO("");
	printf("LIBC main:%p, argc = %d(%p), init = %p, fini = %p, rtld_fini = %p, stacn_end = %p",
		   main, argc, argv, init, fini, rtld_fini, stack_end);

	r = main(argc, argv, 0);

	exit(r);	
}

static void lock_image_list()
{
	//TODO
}

static void unlock_image_list()
{
	//TODO
}

static void relink_dep(struct dl_structure *old_dl, struct image *old_image, struct image *new_image)
{
	int i;
	struct dependency_list *node, *new_node;

	INIT_LIST_HEAD(&new_image->dep_list);
	list_for_each_entry(node, &old_image->dep_list, list)
	{
		i = node - old_dl->static_dependency_pool;
		new_node = dl.static_dependency_pool + i;
		
		list_add_tail(&new_node->list, &new_image->dep_list);
	}
}

DLLEXPORT void *crt_ld_unload_bootstrap(void *old_cmdline)
{
	_unmap_exe_file(old_cmdline);
	
	/* Got new cmdline */
	return dl.cmdline_buffer;
} 

void dl_handle_over(void *old_dl)
{
	int i;
	struct dl_structure *old = old_dl;
	struct image *image, *new_image;
	
	dl = *old;
	INIT_LIST_HEAD(&dl.image_list_head);
	dl.normal_mode = 1;
	
	list_for_each_entry(image, &old->image_list_head, linear_list)
	{
		i = image - old->static_image_pool;
		new_image = dl.static_image_pool + i;
									
		list_add_tail(&new_image->linear_list, &dl.image_list_head);
		relink_dep(old, image, new_image);
		
		/* 可执行文件的名字来自cmdline buffer */
		if (new_image->name >= old->cmdline_buffer && new_image->name < old->cmdline_buffer + sizeof(old->cmdline_buffer))
			new_image->name = dl.cmdline_buffer + (int)(new_image->name - old->cmdline_buffer);
		
	}
}

void *dl_open(const char *name, int mode)
{
	struct image *got;
	
	lock_image_list();
	if ((got = _get_obj_from_linear_list((char*)name)) != NULL)
		goto end;

	/* Itself and its dependencies */
	if (!(got = load_image((xstring)name)))
		goto end;
	load_dependencies();
	
	got->mode = mode;
	relocation();

end:
	unlock_image_list();
	return got;
}

bool dl_close(void *handle)
{
	struct image *image = handle;

	lock_image_list();
	unload_image(image);
	unlock_image_list();
	
	return true;
}

void *dl_sym(void *handle, const char *name)
{
	struct image *image = handle;
	unsigned long addr;
	
	if (ki_get_symbol(&image->exe_desc, name, &addr, 0) == true)
		return (void *)addr;
	
	return NULL;
}

void *dl_entry(void *handle)
{
	struct image *image = handle;
	unsigned long entry;

	if (elf_get_startup(&image->exe_desc, &image->user_ctx, &entry, NULL, NULL) == false)
		return NULL;

	return (void*)entry;
}

void *dl_section_vaddress(void *handle, const char *section_name, size_t *size)
{
	int section_for = -1;
	struct image *image = handle;
	
	/* 目前支持的有限 */
	if (!strcmp(section_name, ".eh_frame"))
		section_for = ELF_SECTION_FOR_EH_FRAME;
	if (section_for == -1)
		return NULL;
	return (void*)elf_get_section_vaddr(&image->exe_desc, &image->user_ctx, section_for, size);
}
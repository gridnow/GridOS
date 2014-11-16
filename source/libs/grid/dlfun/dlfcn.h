/**
	The Grid Core Library
*/

/**
	Posix shared object operations
	Yaosihai
*/

#ifndef _DLFCN_H
#define _DLFCN_H

#define GLOBAL_HANDLE (void*)1

/* dlopen.c */
bool init_module();

/* dl_crt.c */
void dl_handle_over(void *old_dl);
void *dl_open(const char *name, int mode);
bool dl_close(void *handle);
void *dl_sym(void *handle, const char *name);
void *dl_entry(void *handle);
void dl_call_posix_entry(void *entry, int argc, char *argv[]);
void *dl_section_vaddress(void *handle, const char *name, size_t *size);
#endif
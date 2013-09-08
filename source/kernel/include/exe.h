#ifndef KP_EXE_H
#define KP_EXE_H


#include <sys/elf.h>

struct ko_section;
struct ko_process;

struct ko_exe
{
	struct ko_section *backend;
};

#define KO_EXE_TO_PRIVATE(EXE) ((EXE) + 1/*Just after the object*/)

/**
	@brief Create an EXE object
 
	@note
		Caller make sure ctx is at proper size
*/
struct ko_exe *kp_exe_create(struct ko_section *backend, void *ctx);

/**
	@brief 一般从文件创立exe对象
*/
struct ko_exe *kp_exe_create_from_file(xstring name, void *ctx);

/**
	@brief Bind an EXE object to process

	@return
		由于内存不足可能绑定不成功
*/
bool kp_exe_bind(struct ko_process *who, struct ko_exe *what);

/**
	@brief 把EXE共享到本地
*/
bool kp_exe_share(struct ko_process *where, struct ko_section *ks_dst, unsigned long to, struct ko_exe *ke_src);

/**
	&brief Get the context size for validation
*/
int kp_exe_get_context_size();

/**
	@brief 拷贝可执行文件信息到用户空间，用户快速使用该可执行文件 
*/
bool kp_exe_copy_private(struct ko_exe *ke, void *dst_ctx, int dst_size);

/**
	@brief Create a temp one
 
	@note
		Used only during system startup to create the fake exe object for analyzing first user image
*/
struct ko_exe *kp_exe_create_temp();

/**
	@brief Init the EXE module
*/
void kp_exe_init();


#endif
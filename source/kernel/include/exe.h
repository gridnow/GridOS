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
	@brief һ����ļ�����exe����
*/
struct ko_exe *kp_exe_create_from_file(xstring name, void *ctx);

/**
	@brief Bind an EXE object to process

	@return
		�����ڴ治����ܰ󶨲��ɹ�
*/
bool kp_exe_bind(struct ko_process *who, struct ko_exe *what);

/**
	@brief ��EXE��������
*/
bool kp_exe_share(struct ko_process *where, struct ko_section *ks_dst, unsigned long to, struct ko_exe *ke_src);

/**
	&brief Get the context size for validation
*/
int kp_exe_get_context_size();

/**
	@brief ������ִ���ļ���Ϣ���û��ռ䣬�û�����ʹ�øÿ�ִ���ļ� 
*/
bool kp_exe_copy_private(struct ko_exe *ke, void *dst_ctx, int dst_size);

/**
	@brief Create a temp one
 
	@note
		Used only during system startup to create the fake exe object for analyzing first user image
*/
struct ko_exe *kp_exe_create_temp();

/**
	@brief Open the exe object
*/
struct ko_exe *kp_exe_open_by_name(struct ko_process *who, xstring name);

/**
	@brief Init the EXE module
*/
void kp_exe_init();


#endif
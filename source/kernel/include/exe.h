#ifndef KP_EXE_H
#define KP_EXE_H

struct ko_section;
struct ko_process;

struct ko_exe
{
	struct ko_section *backend;
};

/**
	@brief Create an EXE object
*/
struct ko_exe *kp_exe_create(struct ko_section *backend);

/**
	@brief Bind an EXE object to process
*/
void kp_exe_bind(struct ko_process *who, struct ko_exe *what);

/**
	@brief Init the exe module
*/
void kp_exe_init();
#endif
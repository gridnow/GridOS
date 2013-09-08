#ifndef ELF_H
#define ELF_H

struct elf_segment
{
	/* Input */
	int id;

	/* Output */
	unsigned int flags;
	unsigned long log_start;
	unsigned long offset_in_file;
	int size_in_file;
	int size_in_log;
};
#define ELF_SEG_READ			(1 << 0)
#define ELF_SEG_WRITE			(1 << 1)
#define ELF_SEG_EXE				(1 << 2)
#define ELF_SEG_HAS_UNINITED	(1 << 3)

/**
	@brief Get the mapping base the image wants to run

	@param[in] elf 有效的ELF描述符
	@param[in,out] mem_size 在进程空中将占用的内存尺寸
	@param[in] mapping_base 在系统进程中所占用的地址

	@return the 在进程空间中的起始地址，如果为0，则要求上层动态指定地址

	@note
		本函数只是读取描述符，不操作文件影射内存
*/
unsigned long elf_get_mapping_base(void * elf, unsigned long * mem_size, unsigned long * mapping_base);

/**
	@brief 遍历所有装载段

	@return
		bool
	@note
		本函数只是读取描述符，不操作文件影射内存
*/
int elf_read_segment(void *elf, struct elf_segment *seg, int id);

/**
	@brief 分析ELF在进程地址空间中必要的数据

	Generates an ELF descriptor after analyzing

	@param[in] image_buffer the image file starting address
	@param[in] image_size the image file size
	@param[in] the space to store the result

	@return
		bool
*/
int elf_analyze(void * file, int size, void **entry_address, void * ctx);

/**
	@brief Get the needed module name

	We use this name to loaded the required module of a given module
*/
char* elf_get_needed(void * elf, int id, void * user_ctx);

/**
	@brief User want to relocate an ELF

	The user process can do relocation on it process
*/
void * elf_relocation(void * elf, void * manager_elf, void * user_ctx);

/**
	@brief Set user lazy linker

	The user may set a function to be called when a symbol is undefined (or unlinked) during the running.
*/
void elf_set_lazy_linker(void * elf, void * func, void * user_ctx);

/**
	@brief Resolve the symbol value in current elf
	
	@param[in] elf the ELF descriptor
	@param[in] name the name of the symbol to resolve
	@param[in,out] address the write back pointer for the address the symbol points to 

	@return
		The address the symbol points to.
		true on success false on failure.
*/
bool elf_resolve_symbol_by_name(void * elf, void * user_ctx, const char * name, unsigned long * address);

/**
	@brief Get the module entry point
	
	模块的入口、出口、构造和析构
*/
bool elf_get_startup(void * elf, void * user_ctx, unsigned long * entry);

/**
	@brief Get the private structure size of the image
*/
int elf_get_private_size();

#endif 
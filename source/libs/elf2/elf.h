#ifndef ELF_H
#define ELF_H

struct elf_segment
{
	/* Input */
	int id;

	/* Output */
	unsigned int flags;
	unsigned long vstart;
	unsigned long foffset;
	int fsize;
	int vsize;
	int align;
};
#define ELF_SEG_READ			(1 << 0)
#define ELF_SEG_WRITE			(1 << 1)
#define ELF_SEG_EXE				(1 << 2)
#define ELF_SEG_HAS_UNINITED	(1 << 3)

struct elf_section
{
	int section_for;

	size_t size;
	unsigned int flags;
	unsigned long vstart;
};
#define ELF_SECTION_FOR_EH_FRAME 1

/**
	@brief Elf ���û��������
 */
struct elf_user_ctx
{
	unsigned long base;
};

/**
	@brief Get the mapping base the image wants to run

	@param[in] elf ��Ч��ELF������
	@param[in,out] mem_size �ڽ��̿��н�ռ�õ��ڴ�ߴ�
	@param[in] mapping_base ��ϵͳ��������ռ�õĵ�ַ

	@return the �ڽ��̿ռ��е���ʼ��ַ�����Ϊ0����Ҫ���ϲ㶯ָ̬����ַ

	@note
		������ֻ�Ƕ�ȡ���������������ļ�Ӱ���ڴ�
*/
unsigned long elf_get_mapping_base(void *elf, unsigned long *mem_size, unsigned long *mapping_base);

/**
	@brief ��������װ�ض�

	@return
		bool
	@note
		������ֻ�Ƕ�ȡ���������������ļ�Ӱ���ڴ�
*/
int elf_read_segment(void *elf, struct elf_segment *seg, int id);

/**
	@brief ����ELF�ڽ��̵�ַ�ռ��б�Ҫ������

	Generates an ELF descriptor after analyzing

	@param[in] image_buffer the image file starting address
	@param[in] image_size the image file size
	@param[in] the space to store the result

	@return
		bool
*/
int elf_analyze(void *file, int size, void **entry_address, void *ctx);

/**
	@brief Get the needed module name

	We use this name to loaded the required module of a given module
*/
char *elf_get_needed(void *elf, int id, void *user_ctx);

/**
	@brief User want to relocate an ELF

	The user process can do relocation on it process
*/
void elf_relocation(void *elf, void *manager_elf, void *user_ctx);

/**
	@brief Set user lazy linker

	The user may set a function to be called when a symbol is undefined (or unlinked) during the running.
*/
void elf_set_lazy_linker(void *elf, void *func, void *user_ctx);

bool elf_relocate_by_id(void *elf, int rel_id, struct elf_user_ctx *user_ctx);

/**
	@brief Resolve the symbol value in current elf
	
	@param[in] elf the ELF descriptor
	@param[in] name the name of the symbol to resolve
	@param[in,out] address the write back pointer for the address the symbol points to 

	@return
		The address the symbol points to.
		true on success false on failure.
*/
bool elf_resolve_symbol_by_name(void *elf, void *user_ctx, const char *name, unsigned long *address);

/**
	@brief Get the module entry point
	
	ģ�����ڡ����ڡ����������
*/
bool elf_get_startup(void *elf, void *user_ctx, unsigned long *entry, unsigned long *init, unsigned long *fini);

/**
	@brief Get the section address by section flag
 */
unsigned long elf_get_section_vaddr(void *elf, void *user_ctx, int section_for, size_t *size);

/**
	@brief Get the private structure size of the image
*/
int elf_get_private_size();

#endif 
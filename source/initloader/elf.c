/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

#include <types.h>
#include <stddef.h>

#include "elf.h"

void *load_elf_from_mem(void *elf_mem)
{
    Elf(Ehdr) *elf_header = (Elf(Ehdr) *)elf_mem;
    
    /* check ELF header */
    if (elf_header->e_ident[EI_MAG0] != ELFMAG0 ||
        elf_header->e_ident[EI_MAG1] != ELFMAG1 ||
        elf_header->e_ident[EI_MAG2] != ELFMAG2 ||
        elf_header->e_ident[EI_MAG3] != ELFMAG3)
    {
        return NULL;
    }
    /* version */
    if (elf_header->e_version != EV_CURRENT)
    {
        /* invalid version */
        return NULL;
    }
#ifdef __i386__
    /* machine type */
    if (elf_header->e_machine != EM_386 )
    {
        /* invalid machine type */
        return NULL;
    }
#endif
    
    return (void*)((Elf(Addr))elf_mem + (elf_header->e_entry));
}

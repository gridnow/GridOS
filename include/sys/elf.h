/**
*  @defgroup ELF
*  @ingroup ExtSysInterface
*
*  ELF∂®“Â
*
*  @{
*/

#ifndef _SYS_ELF_H
#define _SYS_ELF_H

/* 32-bit ELF base types. */
typedef u32	Elf32_Addr;
typedef u16	Elf32_Half;
typedef u32	Elf32_Off;
typedef s32	Elf32_Sword;
typedef u32	Elf32_Word;

/* 64-bit ELF base types. */
typedef u64	Elf64_Addr;
typedef u16	Elf64_Half;
typedef s16	Elf64_SHalf;
typedef u64	Elf64_Off;
typedef s32	Elf64_Sword;
typedef u32	Elf64_Word;
typedef u64	Elf64_Xword;
typedef s64	Elf64_Sxword;

#define EI_NIDENT	16
/* In Ident */
#define EI_CLASS	4	/* File class */
#define ELFCLASSNONE	      0	/* Invalid class */
#define ELFCLASS32	      1	/* 32-bit objects */
#define ELFCLASS64	      2	/* 64-bit objects */

typedef struct elf32_hdr{
  unsigned char	e_ident[EI_NIDENT];
  Elf32_Half	e_type;
  Elf32_Half	e_machine;
  Elf32_Word	e_version;
  Elf32_Addr	e_entry;  /* Entry point */
  Elf32_Off	e_phoff;
  Elf32_Off	e_shoff;
  Elf32_Word	e_flags;
  Elf32_Half	e_ehsize;
  Elf32_Half	e_phentsize;
  Elf32_Half	e_phnum;
  Elf32_Half	e_shentsize;
  Elf32_Half	e_shnum;
  Elf32_Half	e_shstrndx;
} Elf32_Ehdr;

typedef struct elf64_hdr {
  unsigned char	e_ident[EI_NIDENT];	/* ELF "magic number" */
  Elf64_Half e_type;
  Elf64_Half e_machine;
  Elf64_Word e_version;
  Elf64_Addr e_entry;		/* Entry point virtual address */
  Elf64_Off e_phoff;		/* Program header table file offset */
  Elf64_Off e_shoff;		/* Section header table file offset */
  Elf64_Word e_flags;
  Elf64_Half e_ehsize;
  Elf64_Half e_phentsize;
  Elf64_Half e_phnum;
  Elf64_Half e_shentsize;
  Elf64_Half e_shnum;
  Elf64_Half e_shstrndx;
} Elf64_Ehdr;

#endif
/** @} */


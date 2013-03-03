/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

#ifndef ELF_H
#define	ELF_H

#ifdef _DEBUG
#ifdef __i386__
#undef __i386__
#endif
#define __i386__
#endif

#ifdef	__i386__
#define Elf(t)	Elf32_##t
typedef u32	Elf(Addr);
typedef u16	Elf(Half);
typedef u32	Elf(Off);
typedef s32	Elf(SWord);
typedef u32	Elf(Word);
#else
#define Elf(t)	Elf64##t
#endif

#define EI_MAG0			0
#define EI_MAG1			1
#define EI_MAG2			2
#define EI_MAG3			3
#define EI_CLASS		4
#define EI_DATA			5
#define EI_VERSION		6
#define EI_PAD			7
#define EI_NIDENT		16

#define ELFMAG0			0x7f
#define ELFMAG1			'E'
#define ELFMAG2			'L'
#define ELFMAG3			'F'

#define ELFCLASSNONE	0
#define ELFCLASS32		1
#define ELFCLASS64		2

#define ELFDATANONE		0
#define ELFDATA2LSB		1
#define ELFDATA2MSB		2

#define ET_NONE			0
#define ET_REL			1
#define ET_EXEC			2
#define ET_DYN			3
#define ET_CORE			4
#define ET_LOPROC		0xff00
#define ET_HIPROC		0xffff

#define EM_NONE		  0	/* No machine */
#define EM_M32		  1	/* AT&T WE 32100 */
#define EM_SPARC	  2	/* SUN SPARC */
#define EM_386		  3	/* Intel 80386 */
#define EM_68K		  4	/* Motorola m68k family */
#define EM_88K		  5	/* Motorola m88k family */
#define EM_486		  6	/* Intel 80486 *//* Reserved for future use */
#define EM_860		  7	/* Intel 80860 */
#define EM_MIPS		  8	/* MIPS R3000 (officially, big-endian only) */
#define EM_S370		  9	/* IBM System/370 */
#define EM_MIPS_RS3_LE	 10	/* MIPS R3000 little-endian (Oct 4 1999 Draft) Deprecated */

#define EM_PARISC	 15	/* HPPA */

#define EM_VPP550	 17	/* Fujitsu VPP500 */
#define EM_SPARC32PLUS	 18	/* Sun's "v8plus" */
#define EM_960		 19	/* Intel 80960 */
#define EM_PPC		 20	/* PowerPC */
#define EM_PPC64	 21	/* 64-bit PowerPC */
#define EM_S390		 22	/* IBM S/390 */
#define EM_SPU		 23	/* Sony/Toshiba/IBM SPU */

#define EM_V800		 36	/* NEC V800 series */
#define EM_FR20		 37	/* Fujitsu FR20 */
#define EM_RH32		 38	/* TRW RH32 */
#define EM_MCORE	 39	/* Motorola M*Core */ /* May also be taken by Fujitsu MMA */
#define EM_RCE		 39	/* Old name for MCore */
#define EM_ARM		 40	/* ARM */
#define EM_OLD_ALPHA	 41	/* Digital Alpha */
#define EM_SH		 42	/* Renesas (formerly Hitachi) / SuperH SH */
#define EM_SPARCV9	 43	/* SPARC v9 64-bit */
#define EM_TRICORE	 44	/* Siemens Tricore embedded processor */
#define EM_ARC		 45	/* ARC Cores */
#define EM_H8_300	 46	/* Renesas (formerly Hitachi) H8/300 */
#define EM_H8_300H	 47	/* Renesas (formerly Hitachi) H8/300H */
#define EM_H8S		 48	/* Renesas (formerly Hitachi) H8S */
#define EM_H8_500	 49	/* Renesas (formerly Hitachi) H8/500 */
#define EM_IA_64	 50	/* Intel IA-64 Processor */
#define EM_MIPS_X	 51	/* Stanford MIPS-X */
#define EM_COLDFIRE	 52	/* Motorola Coldfire */
#define EM_68HC12	 53	/* Motorola M68HC12 */
#define EM_MMA		 54	/* Fujitsu Multimedia Accelerator */
#define EM_PCP		 55	/* Siemens PCP */
#define EM_NCPU		 56	/* Sony nCPU embedded RISC processor */
#define EM_NDR1		 57	/* Denso NDR1 microprocesspr */
#define EM_STARCORE	 58	/* Motorola Star*Core processor */
#define EM_ME16		 59	/* Toyota ME16 processor */
#define EM_ST100	 60	/* STMicroelectronics ST100 processor */
#define EM_TINYJ	 61	/* Advanced Logic Corp. TinyJ embedded processor */
#define EM_X86_64	 62	/* Advanced Micro Devices X86-64 processor */

#define EM_PDP10	 64	/* Digital Equipment Corp. PDP-10 */
#define EM_PDP11	 65	/* Digital Equipment Corp. PDP-11 */
#define EM_FX66		 66	/* Siemens FX66 microcontroller */
#define EM_ST9PLUS	 67	/* STMicroelectronics ST9+ 8/16 bit microcontroller */
#define EM_ST7		 68	/* STMicroelectronics ST7 8-bit microcontroller */
#define EM_68HC16	 69	/* Motorola MC68HC16 Microcontroller */
#define EM_68HC11	 70	/* Motorola MC68HC11 Microcontroller */
#define EM_68HC08	 71	/* Motorola MC68HC08 Microcontroller */
#define EM_68HC05	 72	/* Motorola MC68HC05 Microcontroller */
#define EM_SVX		 73	/* Silicon Graphics SVx */
#define EM_ST19		 74	/* STMicroelectronics ST19 8-bit cpu */
#define EM_VAX		 75	/* Digital VAX */
#define EM_CRIS		 76	/* Axis Communications 32-bit embedded processor */
#define EM_JAVELIN	 77	/* Infineon Technologies 32-bit embedded cpu */
#define EM_FIREPATH	 78	/* Element 14 64-bit DSP processor */
#define EM_ZSP		 79	/* LSI Logic's 16-bit DSP processor */
#define EM_MMIX		 80	/* Donald Knuth's educational 64-bit processor */
#define EM_HUANY	 81	/* Harvard's machine-independent format */
#define EM_PRISM	 82	/* SiTera Prism */
#define EM_AVR		 83	/* Atmel AVR 8-bit microcontroller */
#define EM_FR30		 84	/* Fujitsu FR30 */
#define EM_D10V		 85	/* Mitsubishi D10V */
#define EM_D30V		 86	/* Mitsubishi D30V */
#define EM_V850		 87	/* NEC v850 */
#define EM_M32R		 88	/* Renesas M32R (formerly Mitsubishi M32R) */
#define EM_MN10300	 89	/* Matsushita MN10300 */
#define EM_MN10200	 90	/* Matsushita MN10200 */
#define EM_PJ		 91	/* picoJava */
#define EM_OPENRISC	 92	/* OpenRISC 32-bit embedded processor */
#define EM_ARC_A5	 93	/* ARC Cores Tangent-A5 */
#define EM_XTENSA	 94	/* Tensilica Xtensa Architecture */
#define EM_IP2K		101	/* Ubicom IP2022 micro controller */
#define EM_CR		103	/* National Semiconductor CompactRISC */
#define EM_MSP430	105	/* TI msp430 micro controller */
#define EM_BLACKFIN	106	/* ADI Blackfin */
#define EM_ALTERA_NIOS2	113	/* Altera Nios II soft-core processor */
#define EM_CRX		114	/* National Semiconductor CRX */
#define EM_CR16		115	/* National Semiconductor CompactRISC - CR16 */
#define EM_SCORE        135     /* Sunplus Score */

#define EV_NONE			0
#define EV_CURRENT		1

#define SHN_UNDEF		0
#define SHN_LORESERVE	0xff00
#define SHN_LOPROC		0xff00
#define SHN_HIPROC		0xff1f
#define SHN_ABS			0xfff1
#define SHN_COMMON		0xfff2
#define SHN_HIRESERVE	0xffff

typedef struct
{
	unsigned char	e_ident[EI_NIDENT];
	Elf(Half)		e_type;
	Elf(Half)		e_machine;
	Elf(Word)		e_version;
	Elf(Addr)		e_entry;
	Elf(Off)		e_phoff;
	Elf(Off)		e_shoff;
	Elf(Word)		e_flags;
	Elf(Half)		e_ehsize;
	Elf(Half)		e_phentsize;
	Elf(Half)		e_phnum;
	Elf(Half)		e_shentsize;
	Elf(Half)		e_shnum;
	Elf(Half)		e_shstrndx;
} Elf(Ehdr);

#define SHT_NULL		0
#define SHT_PROGBITS	1
#define SHT_SYMTAB		2
#define SHT_STRTAB		3
#define SHT_RELA		4
#define SHT_HASH		5
#define SHT_DYNAMIC		6
#define SHT_NOTE		7
#define SHT_NOBITS		8
#define SHT_REL			9
#define SHT_SHLIB		10
#define SHT_DYNSYM		11
#define SHT_LOPROC		0x70000000L
#define SHT_HIPROC		0x7fffffffL
#define SHT_LOUSER		0x80000000L
#define SHT_HIUSER		0xffffffffL

#define SHF_WRITE		0x1
#define SHF_ALLOC		0x2
#define SHF_EXECINSTR	0x4
#define SHF_MASKPROC	0xf0000000L

typedef struct  
{
	Elf(Word)		sh_name;
	Elf(Word)		sh_type;
	Elf(Word)		sh_flags;
	Elf(Addr)		sh_addr;
	Elf(Off)		sh_offset;
	Elf(Word)		sh_size;
	Elf(Word)		sh_link;
	Elf(Word)		sh_info;
	Elf(Word)		sh_addralign;
	Elf(Word)		sh_entsize;
} Elf(Shdr);

#define STN_UNDEF	0
#define ELF32_ST_BIND(i)	((i) >> 4)
#define ELF32_ST_TYPE(i)	((i) & 0xf)
#define ELF32_ST_INFO(b,t)	(((b) << 4) + ((t) & 0xf))

#define STB_LOCAL		0
#define STB_GLOBAL		1
#define STB_WEAK		2
#define STB_LOPROC		13
#define STB_HIPROC		15

#define STT_NOTYPE		0
#define STT_OBJECT		1
#define STT_FUNC		2
#define STT_SECTION		3
#define STT_FILE		4
#define STT_LOPROC		13
#define STT_HIPROC		15

typedef struct 
{
	Elf(Word)		st_name;
	Elf(Addr)		st_value;
	Elf(Word)		st_size;
	unsigned char	st_info;
	unsigned char	st_other;
	Elf(Half)		st_shndx;
} Elf(Sym);


#define ELF32_R_SYM(i)		((i) >> 8)
#define ELF32_R_TYPE(i)		((unsigned char)(i))
#define ELF32_R_INFO(s,t)	(((s) << 8) + (unsigned char)(t))

typedef struct 
{
	Elf(Addr)		r_offset;
	Elf(Word)		r_info;
} Elf(Rel);

typedef struct 
{
	Elf(Addr)		r_offset;
	Elf(Word)		r_info;
	Elf(SWord)		r_addend;
} Elf(Rela);

#define PT_NULL			0
#define PT_LOAD			1
#define PT_DYNAMIC		2
#define PT_INTERP		3
#define PT_NOTE			4
#define PT_SHLIB		5
#define PT_PHDR			6
#define PT_TLS			7
#define PT_LOOS			0x60000000	/* OS-specific */
#define PT_HIOS			0x6fffffff	/* OS-specific */
#define PT_LOPROC		0x70000000L
#define PT_HIPROC		0x7fffffffL
#define PT_GNU_EH_FRAME	(PT_LOOS + 0x474e550) /* Frame unwind information */
#define PT_SUNW_EH_FRAME PT_GNU_EH_FRAME      /* Solaris uses the same value */
#define PT_GNU_STACK	(PT_LOOS + 0x474e551) /* Stack flags */
#define PT_GNU_RELRO	(PT_LOOS + 0x474e552) /* Read-only after relocation */

#define PF_X			1
#define PF_W			2
#define PF_R			4
#define PF_MASKPROC		0xf0000000L

typedef struct {
	Elf(Word)		p_type;
	Elf(Off)		p_offset;
	Elf(Addr)		p_vaddr;
	Elf(Addr)		p_paddr;
	Elf(Word)		p_filesz;
	Elf(Word)		p_memsz;
	Elf(Word)		p_flags;
	Elf(Word)		p_align;
} Elf(Phdr);

#define DT_NULL			0
#define DT_NEEDED		1
#define DT_PLTRELSZ		2
#define DT_PLTGOT		3
#define DT_HASH			4
#define DT_STRTAB		5
#define DT_SYMTAB		6
#define DT_RELA			7
#define DT_RELASZ		8
#define DT_RELAENT		9
#define DT_STRSZ		10
#define DT_SYMENT		11
#define DT_INIT			12
#define DT_FINI			13
#define DT_SONAME		14
#define DT_RPATH		15
#define DT_SYMBOLIC		16
#define DT_REL			17
#define DT_RELSZ		18
#define DT_RELENT		19
#define DT_PLTREL		20
#define DT_DEBUG		21
#define DT_TEXTREL		22
#define DT_JMPREL		23
#define DT_BIND_NOW		24
#define DT_LOPROC		0x70000000
#define DT_HIPROC		0x7fffffff

typedef struct {
	Elf(SWord) d_tag;
	union {
		Elf(Word)	d_val;
		Elf(Addr)	d_ptr;
	} d_un;
} Elf(Dyn);

#endif	/* ELF_H */

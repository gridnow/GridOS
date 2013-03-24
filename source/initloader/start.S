/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

#define ASM 1
#include "grub.h"

    .text
    
    .globl start, _start
start:
_start:
    jmp multiboot_entry
            
    .align 4
multiboot_header:
    .long   MULTIBOOT_HEADER_MAGIC
    .long   MULTIBOOT_HEADER_FLAGS
    .long   -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
    
multiboot_entry:
    movl $(stack + STACK_SIZE), %esp
    
    pushl $0
    popf
            
    pushl %ebx
    pushl %eax
         
    call EXT_C(init_main)
            
halt:
    hlt
    jmp halt

            
    .comm stack, STACK_SIZE
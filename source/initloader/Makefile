#
# See the readme.txt at the root directory of this project for the idea and originality of this operating system.
# See the license.txt at the root directory of this project for the copyright information about this file and project.
#


include ../../config.mk

my_cflags   += -fno-builtin

OBJS	=   loader.o	\
	    elf.o	\
	    utils.o
	
ASM_OBJS=   start.o

TARGET	=   initloader

all: $(OBJS) $(ASM_OBJS)
	$(LD) -nostdlib -T ldr.lds -o $(TARGET) $(ASM_OBJS) $(OBJS)

include ../../cc.mk
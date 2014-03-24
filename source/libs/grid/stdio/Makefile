DLL=yes
include $(MY_BUILD_BASE)/config.mk

#要编译的文件列表
OBJS = printf.o 
OBJS += fopen.o stream_file.o nocache_file.o
OBJS += ftruncate.o fread.o fclose.o fwrite.o
OBJS += getch.o

ADD_INCLUDE +=-I../include
ADD_INCLUDE += $(CL_INCLUDE)

all:$(OBJS)
	$(LD) -r -o ../stdio.o *.o $(ARCH_LD_FLAGS)

	
include $(MY_BUILD_BASE)/cc.mk

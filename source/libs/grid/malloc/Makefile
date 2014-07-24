DLL=yes
include $(MY_BUILD_BASE)/config.mk


#要编译的文件列表
OBJS = malloc.o mmap.o
ADD_INCLUDE +=$(CL_INCLUDE) -I../include

all:$(OBJS)
	$(LD) -r -o ../malloc.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

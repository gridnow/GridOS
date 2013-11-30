DLL=yes
include $(MY_BUILD_BASE)/config.mk

#要编译的文件列表
OBJS = setenv.o getenv.o exit.o
ADD_INCLUDE +=-I../include
ADD_INCLUDE += $(CL_INCLUDE)

all:$(OBJS)
	$(LD) -r -o ../stdlib.o *.o $(ARCH_LD_FLAGS)

	
include $(MY_BUILD_BASE)/cc.mk

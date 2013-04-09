include $(MY_BUILD_BASE)/config.mk
include ../kernel.mk

ADD_INCLUDE +=-I../include $(HAL_INCLUDE)

#要编译的文件列表
OBJS	= pa.o percpu.o
PARTS	= 

all:  subs_parts $(OBJS)
	$(LD) -r -o ../memory.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

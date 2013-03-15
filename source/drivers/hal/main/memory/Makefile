include $(MY_BUILD_BASE)/config.mk
include ../../hal_include.mk

ADD_INCLUDE +=-I../include $(HAL_INCLUDE) -I$(MY_BUILD_BASE)/source/libs/common/include

#要编译的文件列表
OBJS	= pa.o percpu.o
PARTS	= 

all:  subs_parts $(OBJS)
	$(LD) -r -o ../memory.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

include $(MY_BUILD_BASE)/config.mk
include ../../../../../hal_include.mk

ADD_INCLUDE +=$(HAL_INCLUDE) -include config.h

#要编译的文件列表
OBJS = irq.o
PARTS =

all:  subs_parts $(OBJS)
	ld -r -o ../2f.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

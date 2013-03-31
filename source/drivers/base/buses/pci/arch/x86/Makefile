include $(MY_BUILD_BASE)/config.mk

ADD_INCLUDE += -I../.. -I../../../.. -include configs.h

#要编译的文件列表
OBJS	= init.o direct.o early.o legacy.o common.o bus_numa.o irq.o i386.o
PARTS	= 

all:  subs_parts $(OBJS)
	$(LD) -r -o ../x86.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

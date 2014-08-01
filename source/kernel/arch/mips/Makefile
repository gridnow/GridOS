include $(MY_BUILD_BASE)/config.mk
include ../../kernel.mk

ADD_INCLUDE +=$(HAL_INCLUDE) -include hal_config.h

#要编译的文件列表
OBJS = genex.o setup.o traps.o fault.o thread.o page.o
PARTS = 

all:  subs_parts $(OBJS)
	$(LD) -r -o ../mips.o *.o $(ARCH_LD_FLAGS)


include $(MY_BUILD_BASE)/cc.mk

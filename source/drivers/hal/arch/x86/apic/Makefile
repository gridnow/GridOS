include $(MY_BUILD_BASE)/config.mk
include ../../../hal_include.mk

ADD_INCLUDE +=$(HAL_INCLUDE)

#要编译的文件列表
OBJS 	= apic.o
PARTS 	= 

all:  subs_parts $(OBJS)
	$(LD) -r -o ../apic.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

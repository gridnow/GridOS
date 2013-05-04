include $(MY_BUILD_BASE)/config.mk
include ../../hal_include.mk

ADD_INCLUDE +=$(HAL_INCLUDE) -include hal_config.h -I$(MY_BUILD_BASE)/source/kernel/include/arch/arm

#要编译的文件列表
OBJS 	= head.o serial.o setup.o mmu.o
PARTS 	=

all:  subs_parts $(OBJS)
	$(LD) -r -o ../arm.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

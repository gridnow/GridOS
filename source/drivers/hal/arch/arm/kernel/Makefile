include $(MY_BUILD_BASE)/config.mk
include ../../../hal_include.mk

ADD_INCLUDE +=$(HAL_INCLUDE) -include hal_config.h -I$(MY_BUILD_BASE)/source/kernel/include/arch/arm
ADD_INCLUDE +=-I$(MY_BUILD_BASE)/source/drivers/bsp/arm/include

#要编译的文件列表
OBJS 	= head.o serial.o setup.o mmu.o entry-armv.o fault.o traps.o
OBJS	+=proc-v6.o tlb-v6.o copypage-v6.o cache-v6.o abort-ev6.o pabort-v6.o
PARTS 	=

all:  subs_parts $(OBJS)
	$(LD) -r -o ../kernel.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

include $(MY_BUILD_BASE)/config.mk
include ../../../../hal/hal_include.mk

ADD_INCLUDE +=$(HAL_INCLUDE)  -I../core/include -Iinclude
#ADD_INCLUDE +=-include hal_config.h 

#要编译的文件列表
OBJS 	= irq.o serial.o
PARTS 	=

all:  subs_parts $(OBJS)
	$(LD) -r -o ../soc-4775.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

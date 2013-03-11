include $(MY_BUILD_BASE)/config.mk
include ../../../drivers/hal/hal_include.mk

#要编译的文件列表
OBJS	= bitmap.o bit.o
PARTS	=
ADD_INCLUDE = -I../include $(HAL_INCLUDE)

#本模块要用CPU特有的指令

all:  subs_parts $(OBJS)
	$(LD) -r -o ../cpu.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

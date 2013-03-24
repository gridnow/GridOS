include $(MY_BUILD_BASE)/config.mk
include ../../../drivers/hal/hal_include.mk

#要编译的文件列表
OBJS	= name.o type.o object.o
PARTS	=
ADD_INCLUDE =-I../include $(HAL_INCLUDE)

all:  subs_parts $(OBJS)
	$(LD) -r -o ../obj.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

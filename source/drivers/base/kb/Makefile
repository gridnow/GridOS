include $(MY_BUILD_BASE)/config.mk
include $(MY_BUILD_BASE)/source/drivers/hal/hal_include.mk
ADD_INCLUDE +=$(HAL_INCLUDE) 


#要编译的文件列表
OBJS	= kb.o
PARTS	= 

all:  subs_parts $(OBJS)
	$(LD) -r -o ../kb.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

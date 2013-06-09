include $(MY_BUILD_BASE)/config.mk
include ../hal_include.mk

ADD_INCLUDE +=$(HAL_INCLUDE) -include hal_config.h

#要编译的文件列表
OBJS	= main.o interface.o obj.o resource.o malloc.o devres.o bootmod.o
PARTS	= net

all:  subs_parts $(OBJS)
	$(LD) -r -o ../main.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

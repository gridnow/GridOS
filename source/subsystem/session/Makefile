include $(MY_BUILD_BASE)/config.mk

#要编译的文件列表
OBJS = main.o stdio.o
ADD_INCLUDE				+= -I$(MY_BUILD_BASE)/source/libs/grid/include

all:  subs_parts $(OBJS)
	$(LD) -r -o ../session.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

include $(MY_BUILD_BASE)/config.mk

ADD_INCLUDE +=$(CL_INCLUDE)

#要编译的文件列表
OBJS	= eth.o
PARTS	=

all:  subs_parts $(OBJS)
	$(LD) -r -o ../ethernet.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

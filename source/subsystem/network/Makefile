include $(MY_BUILD_BASE)/config.mk

ADD_INCLUDE += -I../../libs/grid/include $(CL_INCLUDE)
#要编译的文件列表
OBJS = main.o
PARTS = 

all:  subs_parts $(OBJS)
	$(LD) -r -o ../network.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

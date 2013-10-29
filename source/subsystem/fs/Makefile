include $(MY_BUILD_BASE)/config.mk

ADD_INCLUDE += -I./include -I../../libs/grid/include $(CL_INCLUDE)

#要编译的文件列表
OBJS = main.o block.o map.o tree.o mltt.o


all:  subs_parts $(OBJS)
	$(LD) -r -o ../fs.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

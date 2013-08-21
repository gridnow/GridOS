include $(MY_BUILD_BASE)/config.mk

ADD_INCLUDE += -I./include

#要编译的文件列表
OBJS = main.o block.o map.o tree.o mltt.o


all:  subs_parts $(OBJS)
	$(LD) -r -o ../fs.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

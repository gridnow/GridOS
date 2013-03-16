include $(MY_BUILD_BASE)/config.mk

#要编译的文件列表
OBJS	= blkbuf.o
PARTS	= object misc cpu
ADD_INCLUDE +=-Iinclude

all:  subs_parts $(OBJS)
	$(LD) -r -o ../common.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

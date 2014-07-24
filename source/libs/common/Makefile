include $(MY_BUILD_BASE)/config.mk

#要编译的文件列表
OBJS	= blkbuf.o malloc.o i2p.o msg.o ring_buff.o
PARTS	= object misc cpu
ADD_INCLUDE +=-Iinclude $(CL_INCLUDE)

all:  subs_parts $(OBJS)
	$(LD) -r -o ../common.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

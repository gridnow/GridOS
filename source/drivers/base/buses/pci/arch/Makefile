include $(MY_BUILD_BASE)/config.mk

#要编译的文件列表
OBJS	= 
PARTS	= $(ARCH_DIR)

all:  subs_parts $(OBJS)
	$(LD) -r -o ../arch.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

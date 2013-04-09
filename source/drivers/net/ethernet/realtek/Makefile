include $(MY_BUILD_BASE)/config.mk

#要编译的文件列表
OBJS	= 8139cp.o 
PARTS	=

all:  subs_parts $(OBJS)
	$(LD) -r -o ../realtek.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

include $(MY_BUILD_BASE)/config.mk
include kernel.mk

ADD_INCLUDE +=$(HAL_INCLUDE) -include hal_config.h

#要编译的文件列表
OBJS	= cpu.o lock.o atomic.o
PARTS	= arch memory process

all:  subs_parts $(OBJS)
	$(LD) -r -o kernel.a *.o $(ARCH_LD_FLAGS)
	rm *.o
	cd ../drivers/hal && make link
	
include $(MY_BUILD_BASE)/cc.mk

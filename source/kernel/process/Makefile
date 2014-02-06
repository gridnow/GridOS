include $(MY_BUILD_BASE)/config.mk
include ../kernel.mk

ADD_INCLUDE +=$(HAL_INCLUDE)
LAST_INCLUDE +=-I$(MY_BUILD_BASE)/source/libs/


#要编译的文件列表
OBJS	= thread.o process.o exe.o sched.o sleep.o sync.o msg.o 
PARTS	=

all:  subs_parts $(OBJS)
	$(LD) -r -o ../process.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

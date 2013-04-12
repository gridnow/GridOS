include $(MY_BUILD_BASE)/config.mk
include ../hal_include.mk

ADD_INCLUDE +=$(HAL_INCLUDE) -include hal_config.h

#要编译的文件列表
OBJS	= time.o timer.o rtc.o i8253.o clocksource.o clockevents.o
PARTS	= 

all:  subs_parts $(OBJS)
	$(LD) -r -o ../time.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

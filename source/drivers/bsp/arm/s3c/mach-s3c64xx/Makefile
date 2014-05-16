include $(MY_BUILD_BASE)/config.mk
include ../../../../hal/hal_include.mk

ADD_INCLUDE +=$(HAL_INCLUDE)  -I../../include -I../plat-samsung/include -I./include
#ADD_INCLUDE +=-include hal_config.h 

#要编译的文件列表
OBJS 	= smdk6410.o common.o s3cfb.o
PARTS 	=

all:  subs_parts $(OBJS)
	$(LD) -r -o ../mach-s3c64xx.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

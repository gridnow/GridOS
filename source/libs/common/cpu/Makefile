include $(MY_BUILD_BASE)/config.mk
include ../../../drivers/hal/hal_include.mk

#要编译的文件列表
OBJS	= bitmap.o atomic.o bit.o string.o
PARTS	=
CPU_SPEICAL_LIBS =

ADD_INCLUDE = -I../include $(HAL_INCLUDE)

#CPU Special
ifeq ($(ARCH),arm)
CPU_SPECIAL_LIBS_PART = ../../../drivers/hal/arch/arm/lib
CPU_SPEICAL_LIBS += $(CPU_SPECIAL_LIBS_PART)/memzero.o \
					$(CPU_SPECIAL_LIBS_PART)/lib1funcs.o \
					$(CPU_SPECIAL_LIBS_PART)/findbit.o
endif

#本模块要用CPU特有的指令

all:  subs_parts $(OBJS)
	$(LD) -r -o ../cpu.o *.o $(CPU_SPEICAL_LIBS) $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

include $(MY_BUILD_BASE)/config.mk
include ../../../hal_include.mk

ADD_INCLUDE +=$(HAL_INCLUDE) -include hal_config.h

#要编译的文件列表
OBJS 	= head_$(BITS).o dt.o main.o serial.o io_delay.o tsc.o x86_init.o pci_early.o smp.o
PARTS 	= cpu lib
# smp lib

all:  subs_parts $(OBJS)
	$(LD) -r -o ../x86.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

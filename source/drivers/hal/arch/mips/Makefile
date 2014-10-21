include $(MY_BUILD_BASE)/config.mk
include ../../hal_include.mk

ADD_INCLUDE +=$(HAL_INCLUDE)

#要编译的文件列表
OBJS = head.o main.o irq.o i8259.o traps.o irq_cpu.o cpu-probe.o watch.o smp.o dma.o
PARTS = plat lib mm

all:  subs_parts $(OBJS)
	ld -r -o ../mips.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

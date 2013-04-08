include $(MY_BUILD_BASE)/config.mk
include ../../hal_include.mk

ADD_INCLUDE +=$(HAL_INCLUDE) -include hal_config.h

#要编译的文件列表
OBJS 	= head_$(BITS).o dt.o main.o serial.o io_delay.o tsc.o x86_init.o smp.o rtc.o irq.o irq_$(BITS).o misca_$(BITS).o traps.o irqinit.o i8259.o i8253.o time.o
PARTS 	= cpu lib apic

all:  subs_parts $(OBJS)
	$(LD) -r -o ../x86.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

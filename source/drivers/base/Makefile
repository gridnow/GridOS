include $(MY_BUILD_BASE)/config.mk

OBJS					= main.o z_initcall_tail.o _initcall_head.o

PARTS = buses
		
all: subs_parts $(OBJS)
	$(LD) -r -o base.a *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

 
include $(MY_BUILD_BASE)/config.mk

OBJS					= main.o

PARTS = buses
		
all: subs_parts $(OBJS)
	$(LD) -r -o ../hal/base_bus.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

 

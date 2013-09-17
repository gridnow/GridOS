include $(MY_BUILD_BASE)/config.mk

ADD_INCLUDE		+= -I$(MY_BUILD_BASE)/include/kernel
LAST_INCLUDE	+= -I../include
PARTS			= 
OBJS			= vmalloc.o stdio.o
 
all: subs_parts $(OBJS)
	$(LD) -r -o ../sys.o *.o $(ARCH_LD_FLAGS)
	

include $(MY_BUILD_BASE)/cc.mk

 
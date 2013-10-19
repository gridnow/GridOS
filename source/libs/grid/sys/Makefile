include $(MY_BUILD_BASE)/config.mk

LAST_INCLUDE	+= -I../include
PARTS			= 
OBJS			= vmalloc.o stdio.o file.o fb.o
 
all: subs_parts $(OBJS)
	$(LD) -r -o ../sys.o *.o $(ARCH_LD_FLAGS)
	

include $(MY_BUILD_BASE)/cc.mk

 

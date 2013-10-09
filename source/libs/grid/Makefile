DLL=yes
include $(MY_BUILD_BASE)/config.mk

OBJS					= main.o
TARGET					= grid.so
DEPENDS					= -lhead_so.o 
#DEPENDS				+=../../pthread/pthread.o
DEPENDS					+= ../../libs/common/*.o
ADD_INCLUDE				+=-I./include $(CL_INCLUDE)
LDFLAGS					+=-Ttext-segment=0x70000000
PARTS					= crt0 dlfun malloc string stdio sys

all: subs_parts $(OBJS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(ADD_LDFLAGS) $(DEPENDS) *.o $(ARCH_LD_FLAGS) 
	$(CP) $(TARGET) $(SYSTEM_DIR)

include $(MY_BUILD_BASE)/cc.mk

 
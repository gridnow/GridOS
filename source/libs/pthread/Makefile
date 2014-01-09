DLL						=yes

include $(MY_BUILD_BASE)/config.mk

TARGET					= libpthread.so
DEPENDS					= -lhead_so.o
PARTS					= 
OBJS					= pthread.o
ADD_INCLUDE				+=-I../grid/include

all: subs_parts $(OBJS) implement.h
	$(LD) $(LDFLAGS) -o $(TARGET) $(ADD_LDFLAGS) $(DEPENDS) *.o $(ARCH_LD_FLAGS)
	$(CP) $(TARGET) $(SYSTEM_DIR)

include $(MY_BUILD_BASE)/cc.mk

 

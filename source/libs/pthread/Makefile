DLL						=yes

include $(MY_BUILD_BASE)/config.mk

TARGET					= libpthread.so
DEPENDS					= -lhead_so.o -lgrid
PARTS					= 
OBJS					= pthread.o
ADD_INCLUDE				+=-I../grid/include

FROM_OTHER_LIB			=$(MY_BUILD_BASE)/build/pthread_spin_*.o

all: subs_parts $(OBJS) implement.h
	$(LD) $(LDFLAGS) -o $(TARGET) $(ADD_LDFLAGS) $(DEPENDS) *.o $(FROM_OTHER_LIB) $(ARCH_LD_FLAGS)
	$(CP) $(TARGET) $(SYSTEM_DIR)

include $(MY_BUILD_BASE)/cc.mk

 

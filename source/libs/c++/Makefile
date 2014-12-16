DLL						=yes

include $(MY_BUILD_BASE)/config.mk
CC = $(GCC_PREFIX)g++
TARGET					= libgrdc++.a
DEPENDS					= -lgrid -lhead_so.o 
PARTS					= cxxabi libsupc++
OBJS					= 
ADD_INCLUDE				=
all: subs_parts $(OBJS)
	$(AR) -r $(TARGET) *.o
	$(CP) $(TARGET) $(SYSTEM_DIR)

include $(MY_BUILD_BASE)/cc.mk

 

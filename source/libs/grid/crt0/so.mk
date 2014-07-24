DLL=yes
include $(MY_BUILD_BASE)/config.mk

OBJS					= head_so.o
LAST_INCLUDE			+=-I../include
PARTS					= 

all: subs_parts $(OBJS)
	$(CP) $(OBJS) $(SYSTEM_DIR)

include $(MY_BUILD_BASE)/cc.mk

 
include $(MY_BUILD_BASE)/config.mk

#要编译的文件列表
OBJS	=
PARTS	= common grid pthread

all:  shared_first subs_parts $(OBJS)

shared_first:
	echo "building shared public code."
	make -f mk.shared DLL=yes
	make -f mk.shared DLL=yes clean
	
include $(MY_BUILD_BASE)/cc.mk

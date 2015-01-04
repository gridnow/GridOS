include $(MY_BUILD_BASE)/config.mk

#要编译的文件列表
OBJS	= unwind-dw2.o \
			unwind-dw2-fde-darwin.o
PARTS	=
ADD_INCLUDE +=-I. -I$(MY_BUILD_BASE)/include/c++ -I$(MY_BUILD_BASE)/include/c++/std \
				-I$(MY_BUILD_BASE)/include/c++/c_std -I../cxxabi \
				$(STD_INCLUDE)
my_c++flags += -c

all:  subs_parts $(OBJS)
	$(CP) $(OBJS) ../

include $(MY_BUILD_BASE)/cc.mk

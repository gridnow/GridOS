include $(MY_BUILD_BASE)/config.mk

#要编译的文件列表
OBJS	= New_op.o New_opnt.o New_opv.o New_opvnt.o New_handler.o \
			Del_op.o Del_opnt.o Del_opv.o Del_opvnt.o \
			Tinfo.o Tinfo2.o \
			eh_alloc.o eh_throw.o eh_catch.o eh_aux_runtime.o eh_exception.o \
			eh_globals.o eh_personality.o eh_term_handler.o eh_terminate.o \
			eh_unex_handler.o eh_type.o guard.o pure.o vec.o vterminate.o
PARTS	=
ADD_INCLUDE +=-I. -I$(MY_BUILD_BASE)/include/c++ -I$(MY_BUILD_BASE)/include/c++/std \
				-I$(MY_BUILD_BASE)/include/c++/c_std -I../cxxabi \
				$(STD_INCLUDE)
my_c++flags += -c 

all:  subs_parts $(OBJS)
	$(CP) $(OBJS) ../

include $(MY_BUILD_BASE)/cc.mk

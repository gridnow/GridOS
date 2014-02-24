include $(MY_BUILD_BASE)/config.mk

ADD_INCLUDE +=-I../include

OBJS	= etharp.o main.o memp.o netif.o pbuf.o phash.o tcp.o tcp_in.o tcp_out.o udp.o timers.o hash.o
PARTS	= ipv4

all:  subs_parts $(OBJS)
	$(LD) -r -o ../src.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

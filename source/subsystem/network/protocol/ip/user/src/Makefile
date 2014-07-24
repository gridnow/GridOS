include $(MY_BUILD_BASE)/config.mk

ADD_INCLUDE +=-I../include -I../include/arch -I../include/ipv4

OBJS	= etharp.o memp.o netif.o pbuf.o def.o tcp.o tcp_in.o tcp_out.o udp.o timers.o hash.o init.o stats.o
PARTS	= ipv4

all:  subs_parts $(OBJS)
	$(LD) -r -o ../src.o *.o $(ARCH_LD_FLAGS)

include $(MY_BUILD_BASE)/cc.mk

#Mips need ABICALLS for normal app with so linked
NORMAL_APP=yes

include $(MY_BUILD_BASE)/config.mk

NES_SRC=InfoNES093J_Src

OBJS					= $(NES_SRC)/InfoNES.o $(NES_SRC)/InfoNES_Mapper.o $(NES_SRC)/InfoNES_pAPU.o $(NES_SRC)/K6502.o main.o
TARGET					= nes.exe
DEPENDS					+=-lgrid -lpthread -lhead_exe.o  $(NES_SRC)/*.o
ADD_INCLUDE				+=-I$(NES_SRC)
COMMON_CC_FLAGS				+=-fno-exceptions
PARTS					= $(NES_SRC)
all: subs_parts $(OBJS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(ADD_LDFLAGS) $(DEPENDS) *.o $(ARCH_LD_FLAGS)
	$(CP) $(TARGET) $(SYSTEM_DIR)
	$(CP) *.nes $(SYSTEM_DIR)
	
	#because make clean not work for NES_SRC
	rm -f $(NES_SRC)/*.o

include $(MY_BUILD_BASE)/cc.mk

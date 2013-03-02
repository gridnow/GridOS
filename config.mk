#
#通用工程配置文件
#

#
#工具宏
#
CC				= gcc
AS				= as
AR				= ar
LD				= ld
RES				= fares
CP				= cp

CFLAGS_arm         = -c -O2
CFLAGS_i386        = -c -O2
CFGAGS_MIPS64      = -c -O2 -mabi=64 -mips3 -EL -G0 
CFGAGS_MIPS32      = -c -O2 -mabi=32 -mips2 -EL -G0 
STD_INCLUDE        = -I$(MY_BUILD_BASE)/include
COMMON_CC_FLAGS    += -Wimplicit-function-declaration
COMMON_CC_FLAGS    += -fvisibility=hidden

#
#连接选项
#
LDFLAGS				= -L$(SYSTEM_DIR)
ifeq ($(DLL),yes)
LDFLAGS    			+= -shared
endif

#
#项目管理文件配置
#
SYSTEM_BUILD_TMP    = $(MY_BUILD_BASE)/build
SYSTEM_DIR          = $(MY_BUILD_BASE)/release/os/$(ARCH)
SYSTEM_DRV_DIR      = $(SYSTEM_DIR)/Drivers
SYSTEM_PACK_DIR     = $(MY_BUILD_BASE)/tools/kpck

#
#编码处理，默认为GBK字符，宽字符的是UNICODE。设置宏"CODE_ENCODE=utf8"可选择utf8的编码
#
ifeq ($(CODE_ENCODE),utf8)
COMMON_CC_FLAGS			+=
else
COMMON_CC_FLAGS			+= -fexec-charset=gbk -finput-charset=gbk -fwide-exec-charset=UCS-2LE
endif

#
#调试处理
#
ifeq ($(DEBUG),YES)
COMMON_CC_FLAGS 		+=-D__DEBUG__=1
endif

#
#编译命令行合成
#
ifeq ($(ARCH),mips64)
	BITS=64
	COMMON_CC_FLAGS		+=$(CFGAGS_MIPS64) $(STD_INCLUDE)
	ARCH_DIR			=mips
	ARCH_LD_FLAGS		=-melf64ltsmip
 ifeq ($(DLL),yes)
 COMMON_CC_FLAGS    += -mabicalls
 endif
 ifeq ($(NORMAL_APP),yes)
 COMMON_CC_FLAGS    += -mabicalls
 endif
else ifeq ($(ARCH),mips32)
	BITS=32
	COMMON_CC_FLAGS +=  $(CFGAGS_MIPS32) $(STD_INCLUDE)
	ARCH_DIR=mips

else ifeq ($(ARCH),arm)
	BITS=32
	COMMON_CC_FLAGS +=  $(CFLAGS_arm) $(STD_INCLUDE)
	ARCH_DIR=arm

else
	BITS=32
	ARCH=i386
	COMMON_CC_FLAGS += $(CFLAGS_i386) $(STD_INCLUDE)
	ARCH_DIR=x86
endif
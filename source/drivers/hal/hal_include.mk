HAL_INCLUDE = -I$(MY_BUILD_BASE)/source/drivers/hal/include -I$(MY_BUILD_BASE)/source/drivers/hal/include/arch/$(ARCH_DIR) -I$(MY_BUILD_BASE)/source/drivers/hal/include/arch/ -I$(MY_BUILD_BASE)/source/drivers/hal/arch/$(ARCH_DIR)/$(PLAT)/include -I$(MY_BUILD_BASE)/source/drivers/hal/arch/$(ARCH_DIR)/$(MACHINE)/include -I$(MY_BUILD_BASE)/source/libs/common/include -I$(MY_BUILD_BASE)/source/libs/common/include/arch/$(ARCH_DIR) -include hal_config.h
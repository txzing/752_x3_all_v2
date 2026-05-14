################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/xswitch/subdir.mk
-include src/xgpio_i2c/subdir.mk
-include src/xgpio/subdir.mk
-include src/vtc/subdir.mk
-include src/vpss/subdir.mk
-include src/version/subdir.mk
-include src/vdma/subdir.mk
-include src/tpg/subdir.mk
-include src/sil9136/subdir.mk
-include src/serdes/subdir.mk
-include src/qspi_flash/subdir.mk
-include src/ps_i2c/subdir.mk
-include src/ps_gpio/subdir.mk
-include src/pixel_compare/subdir.mk
-include src/modbus_slave/util_lib/subdir.mk
-include src/modbus_slave/axi_uartlite_fifo/subdir.mk
-include src/modbus_slave/subdir.mk
-include src/md5_b/subdir.mk
-include src/lwip_servers/subdir.mk
-include src/it6801/subdir.mk
-include src/hot_reset/subdir.mk
-include src/eeprom/subdir.mk
-include src/demosaic/subdir.mk
-include src/csi_tx/subdir.mk
-include src/csi_rx/subdir.mk
-include src/clk_wiz/subdir.mk
-include src/axi_timer/subdir.mk
-include src/ads7128/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
ELFSIZE += \
vitis_proj.elf.size \


# All Target
all: vitis_proj.elf secondary-outputs

# Tool invocations
vitis_proj.elf: $(OBJS) ../src/lscript.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM v8 gcc linker'
	aarch64-none-elf-gcc -Wl,-Map,vitis_proj.map -Wl,-T -Wl,../src/lscript.ld -LD:/workdir/cariad/ccu/752_x3_all_v2/vitis/sdk_workspace/system_wrapper/export/system_wrapper/sw/system_wrapper/standalone_domain/bsplib/lib -o "vitis_proj.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

vitis_proj.elf.size: vitis_proj.elf
	@echo 'Invoking: ARM v8 Print Size'
	aarch64-none-elf-size vitis_proj.elf  |tee "vitis_proj.elf.size"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(S_UPPER_DEPS)$(C_DEPS)$(ELFSIZE) vitis_proj.elf
	-@echo ' '

secondary-outputs: $(ELFSIZE)

.PHONY: all clean dependents

-include ../makefile.targets

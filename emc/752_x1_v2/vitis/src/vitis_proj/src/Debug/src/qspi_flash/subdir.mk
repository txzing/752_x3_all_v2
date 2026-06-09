################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/qspi_flash/qspi_flash_mb.c \
../src/qspi_flash/qspi_flash_zynq.c \
../src/qspi_flash/qspi_flash_zynqmp.c 

OBJS += \
./src/qspi_flash/qspi_flash_mb.o \
./src/qspi_flash/qspi_flash_zynq.o \
./src/qspi_flash/qspi_flash_zynqmp.o 

C_DEPS += \
./src/qspi_flash/qspi_flash_mb.d \
./src/qspi_flash/qspi_flash_zynq.d \
./src/qspi_flash/qspi_flash_zynqmp.d 


# Each subdirectory must supply rules for building sources it contributes
src/qspi_flash/%.o: ../src/qspi_flash/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v8 gcc compiler'
	aarch64-none-elf-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -ID:/workdir/cariad/ccu/752_x3_all_v2/vitis/sdk_workspace/system_wrapper/export/system_wrapper/sw/system_wrapper/standalone_domain/bspinclude/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



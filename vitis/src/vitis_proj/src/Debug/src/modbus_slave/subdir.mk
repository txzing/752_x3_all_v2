################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/modbus_slave/modbus_slave.c 

OBJS += \
./src/modbus_slave/modbus_slave.o 

C_DEPS += \
./src/modbus_slave/modbus_slave.d 


# Each subdirectory must supply rules for building sources it contributes
src/modbus_slave/%.o: ../src/modbus_slave/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v8 gcc compiler'
	aarch64-none-elf-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -ID:/workdir/cariad/ccu/752_x3_all_v2/vitis/sdk_workspace/system_wrapper/export/system_wrapper/sw/system_wrapper/standalone_domain/bspinclude/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



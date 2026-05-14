################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/csi_rx/csi_rx.c 

OBJS += \
./src/csi_rx/csi_rx.o 

C_DEPS += \
./src/csi_rx/csi_rx.d 


# Each subdirectory must supply rules for building sources it contributes
src/csi_rx/%.o: ../src/csi_rx/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v8 gcc compiler'
	aarch64-none-elf-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -ID:/workdir/cariad/ccu/752_x3_all_v2/vitis/sdk_workspace/system_wrapper/export/system_wrapper/sw/system_wrapper/standalone_domain/bspinclude/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



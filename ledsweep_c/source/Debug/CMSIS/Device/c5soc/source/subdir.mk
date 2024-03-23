################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/Device/c5soc/source/irq_c5soc.c \
../CMSIS/Device/c5soc/source/mmu_c5soc.c \
../CMSIS/Device/c5soc/source/startup_c5soc.c \
../CMSIS/Device/c5soc/source/system_c5soc.c 

OBJS += \
./CMSIS/Device/c5soc/source/irq_c5soc.o \
./CMSIS/Device/c5soc/source/mmu_c5soc.o \
./CMSIS/Device/c5soc/source/startup_c5soc.o \
./CMSIS/Device/c5soc/source/system_c5soc.o 

C_DEPS += \
./CMSIS/Device/c5soc/source/irq_c5soc.d \
./CMSIS/Device/c5soc/source/mmu_c5soc.d \
./CMSIS/Device/c5soc/source/startup_c5soc.d \
./CMSIS/Device/c5soc/source/system_c5soc.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/Device/c5soc/source/%.o: ../CMSIS/Device/c5soc/source/%.c CMSIS/Device/c5soc/source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-a9 -marm -mfloat-abi=hard -mfpu=neon -mno-unaligned-access -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -g3 -DDEBUG -Dsoc_cv_av -DCYCLONEV -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\CMSIS\Core\Include" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\CMSIS\Core\Include\a-profile" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\CMSIS\Device\c5soc\include" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\bsp" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\hwlib\include" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\hwlib\include\soc_cv_av" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\trulib\include" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\trulib_config\include" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\irqhandler\include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



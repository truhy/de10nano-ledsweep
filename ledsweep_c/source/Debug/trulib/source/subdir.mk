################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../trulib/source/etu.c \
../trulib/source/tru_irq.c \
../trulib/source/tru_newlib_ext.c \
../trulib/source/tru_startup.c \
../trulib/source/tru_uart_ll.c \
../trulib/source/tru_util.c 

OBJS += \
./trulib/source/etu.o \
./trulib/source/tru_irq.o \
./trulib/source/tru_newlib_ext.o \
./trulib/source/tru_startup.o \
./trulib/source/tru_uart_ll.o \
./trulib/source/tru_util.o 

C_DEPS += \
./trulib/source/etu.d \
./trulib/source/tru_irq.d \
./trulib/source/tru_newlib_ext.d \
./trulib/source/tru_startup.d \
./trulib/source/tru_uart_ll.d \
./trulib/source/tru_util.d 


# Each subdirectory must supply rules for building sources it contributes
trulib/source/%.o: ../trulib/source/%.c trulib/source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-a9 -marm -mfloat-abi=hard -mfpu=neon -mno-unaligned-access -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -g3 -DDEBUG -Dsoc_cv_av -DCYCLONEV -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\CMSIS\Core\Include" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\CMSIS\Core\Include\a-profile" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\CMSIS\Device\c5soc\include" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\bsp" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\hwlib\include" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\hwlib\include\soc_cv_av" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\trulib\include" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\trulib_config\include" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\irqhandler\include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



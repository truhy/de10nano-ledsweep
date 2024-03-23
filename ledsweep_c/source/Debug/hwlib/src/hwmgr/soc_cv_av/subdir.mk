################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../hwlib/src/hwmgr/soc_cv_av/alt_bridge_manager.c \
../hwlib/src/hwmgr/soc_cv_av/alt_clock_manager.c \
../hwlib/src/hwmgr/soc_cv_av/alt_clock_manager_init.c \
../hwlib/src/hwmgr/soc_cv_av/alt_ecc.c \
../hwlib/src/hwmgr/soc_cv_av/alt_fpga_manager.c \
../hwlib/src/hwmgr/soc_cv_av/alt_reset_manager.c \
../hwlib/src/hwmgr/soc_cv_av/alt_sdram.c \
../hwlib/src/hwmgr/soc_cv_av/alt_system_manager.c 

OBJS += \
./hwlib/src/hwmgr/soc_cv_av/alt_bridge_manager.o \
./hwlib/src/hwmgr/soc_cv_av/alt_clock_manager.o \
./hwlib/src/hwmgr/soc_cv_av/alt_clock_manager_init.o \
./hwlib/src/hwmgr/soc_cv_av/alt_ecc.o \
./hwlib/src/hwmgr/soc_cv_av/alt_fpga_manager.o \
./hwlib/src/hwmgr/soc_cv_av/alt_reset_manager.o \
./hwlib/src/hwmgr/soc_cv_av/alt_sdram.o \
./hwlib/src/hwmgr/soc_cv_av/alt_system_manager.o 

C_DEPS += \
./hwlib/src/hwmgr/soc_cv_av/alt_bridge_manager.d \
./hwlib/src/hwmgr/soc_cv_av/alt_clock_manager.d \
./hwlib/src/hwmgr/soc_cv_av/alt_clock_manager_init.d \
./hwlib/src/hwmgr/soc_cv_av/alt_ecc.d \
./hwlib/src/hwmgr/soc_cv_av/alt_fpga_manager.d \
./hwlib/src/hwmgr/soc_cv_av/alt_reset_manager.d \
./hwlib/src/hwmgr/soc_cv_av/alt_sdram.d \
./hwlib/src/hwmgr/soc_cv_av/alt_system_manager.d 


# Each subdirectory must supply rules for building sources it contributes
hwlib/src/hwmgr/soc_cv_av/%.o: ../hwlib/src/hwmgr/soc_cv_av/%.c hwlib/src/hwmgr/soc_cv_av/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-a9 -marm -mfloat-abi=hard -mfpu=neon -mno-unaligned-access -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -g3 -DDEBUG -Dsoc_cv_av -DCYCLONEV -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\CMSIS\Core\Include" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\CMSIS\Core\Include\a-profile" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\CMSIS\Device\c5soc\include" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\bsp" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\hwlib\include" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\hwlib\include\soc_cv_av" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\trulib\include" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\trulib_config\include" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\irqhandler\include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



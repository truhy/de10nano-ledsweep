################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../hwlib/src/hwmgr/alt_16550_uart.c \
../hwlib/src/hwmgr/alt_address_space.c \
../hwlib/src/hwmgr/alt_cache.c \
../hwlib/src/hwmgr/alt_can.c \
../hwlib/src/hwmgr/alt_dma.c \
../hwlib/src/hwmgr/alt_dma_program.c \
../hwlib/src/hwmgr/alt_generalpurpose_io.c \
../hwlib/src/hwmgr/alt_globaltmr.c \
../hwlib/src/hwmgr/alt_i2c.c \
../hwlib/src/hwmgr/alt_interrupt.c \
../hwlib/src/hwmgr/alt_mmu.c \
../hwlib/src/hwmgr/alt_nand.c \
../hwlib/src/hwmgr/alt_qspi.c \
../hwlib/src/hwmgr/alt_safec.c \
../hwlib/src/hwmgr/alt_sdmmc.c \
../hwlib/src/hwmgr/alt_spi.c \
../hwlib/src/hwmgr/alt_timers.c \
../hwlib/src/hwmgr/alt_watchdog.c 

OBJS += \
./hwlib/src/hwmgr/alt_16550_uart.o \
./hwlib/src/hwmgr/alt_address_space.o \
./hwlib/src/hwmgr/alt_cache.o \
./hwlib/src/hwmgr/alt_can.o \
./hwlib/src/hwmgr/alt_dma.o \
./hwlib/src/hwmgr/alt_dma_program.o \
./hwlib/src/hwmgr/alt_generalpurpose_io.o \
./hwlib/src/hwmgr/alt_globaltmr.o \
./hwlib/src/hwmgr/alt_i2c.o \
./hwlib/src/hwmgr/alt_interrupt.o \
./hwlib/src/hwmgr/alt_mmu.o \
./hwlib/src/hwmgr/alt_nand.o \
./hwlib/src/hwmgr/alt_qspi.o \
./hwlib/src/hwmgr/alt_safec.o \
./hwlib/src/hwmgr/alt_sdmmc.o \
./hwlib/src/hwmgr/alt_spi.o \
./hwlib/src/hwmgr/alt_timers.o \
./hwlib/src/hwmgr/alt_watchdog.o 

C_DEPS += \
./hwlib/src/hwmgr/alt_16550_uart.d \
./hwlib/src/hwmgr/alt_address_space.d \
./hwlib/src/hwmgr/alt_cache.d \
./hwlib/src/hwmgr/alt_can.d \
./hwlib/src/hwmgr/alt_dma.d \
./hwlib/src/hwmgr/alt_dma_program.d \
./hwlib/src/hwmgr/alt_generalpurpose_io.d \
./hwlib/src/hwmgr/alt_globaltmr.d \
./hwlib/src/hwmgr/alt_i2c.d \
./hwlib/src/hwmgr/alt_interrupt.d \
./hwlib/src/hwmgr/alt_mmu.d \
./hwlib/src/hwmgr/alt_nand.d \
./hwlib/src/hwmgr/alt_qspi.d \
./hwlib/src/hwmgr/alt_safec.d \
./hwlib/src/hwmgr/alt_sdmmc.d \
./hwlib/src/hwmgr/alt_spi.d \
./hwlib/src/hwmgr/alt_timers.d \
./hwlib/src/hwmgr/alt_watchdog.d 


# Each subdirectory must supply rules for building sources it contributes
hwlib/src/hwmgr/%.o: ../hwlib/src/hwmgr/%.c hwlib/src/hwmgr/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-a9 -marm -mfloat-abi=hard -mfpu=neon -mno-unaligned-access -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -g3 -DDEBUG -Dsoc_cv_av -DCYCLONEV -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\CMSIS\Core\Include" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\CMSIS\Core\Include\a-profile" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\CMSIS\Device\c5soc\include" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\bsp" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\hwlib\include" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\hwlib\include\soc_cv_av" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\trulib\include" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\trulib_config\include" -I"D:\Documents\Programming\FPGA\de10nano-c-verilog\ledsweep\ledsweep_c\source\irqhandler\include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



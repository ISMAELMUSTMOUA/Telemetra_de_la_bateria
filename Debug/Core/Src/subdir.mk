################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/ad7124.c \
../Core/Src/ad7124_console_app.c \
../Core/Src/ad7124_regs.c \
../Core/Src/ad7124_regs_config_a.c \
../Core/Src/ad7124_regs_config_b.c \
../Core/Src/ad7124_regs_config_c.c \
../Core/Src/ad7124_support.c \
../Core/Src/adi_console_menu.c \
../Core/Src/delay.c \
../Core/Src/main.c \
../Core/Src/platform_support.c \
../Core/Src/spi.c \
../Core/Src/stm32g4xx_hal_msp.c \
../Core/Src/stm32g4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32g4xx.c 

OBJS += \
./Core/Src/ad7124.o \
./Core/Src/ad7124_console_app.o \
./Core/Src/ad7124_regs.o \
./Core/Src/ad7124_regs_config_a.o \
./Core/Src/ad7124_regs_config_b.o \
./Core/Src/ad7124_regs_config_c.o \
./Core/Src/ad7124_support.o \
./Core/Src/adi_console_menu.o \
./Core/Src/delay.o \
./Core/Src/main.o \
./Core/Src/platform_support.o \
./Core/Src/spi.o \
./Core/Src/stm32g4xx_hal_msp.o \
./Core/Src/stm32g4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32g4xx.o 

C_DEPS += \
./Core/Src/ad7124.d \
./Core/Src/ad7124_console_app.d \
./Core/Src/ad7124_regs.d \
./Core/Src/ad7124_regs_config_a.d \
./Core/Src/ad7124_regs_config_b.d \
./Core/Src/ad7124_regs_config_c.d \
./Core/Src/ad7124_support.d \
./Core/Src/adi_console_menu.d \
./Core/Src/delay.d \
./Core/Src/main.d \
./Core/Src/platform_support.d \
./Core/Src/spi.d \
./Core/Src/stm32g4xx_hal_msp.d \
./Core/Src/stm32g4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32g4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/ad7124.cyclo ./Core/Src/ad7124.d ./Core/Src/ad7124.o ./Core/Src/ad7124.su ./Core/Src/ad7124_console_app.cyclo ./Core/Src/ad7124_console_app.d ./Core/Src/ad7124_console_app.o ./Core/Src/ad7124_console_app.su ./Core/Src/ad7124_regs.cyclo ./Core/Src/ad7124_regs.d ./Core/Src/ad7124_regs.o ./Core/Src/ad7124_regs.su ./Core/Src/ad7124_regs_config_a.cyclo ./Core/Src/ad7124_regs_config_a.d ./Core/Src/ad7124_regs_config_a.o ./Core/Src/ad7124_regs_config_a.su ./Core/Src/ad7124_regs_config_b.cyclo ./Core/Src/ad7124_regs_config_b.d ./Core/Src/ad7124_regs_config_b.o ./Core/Src/ad7124_regs_config_b.su ./Core/Src/ad7124_regs_config_c.cyclo ./Core/Src/ad7124_regs_config_c.d ./Core/Src/ad7124_regs_config_c.o ./Core/Src/ad7124_regs_config_c.su ./Core/Src/ad7124_support.cyclo ./Core/Src/ad7124_support.d ./Core/Src/ad7124_support.o ./Core/Src/ad7124_support.su ./Core/Src/adi_console_menu.cyclo ./Core/Src/adi_console_menu.d ./Core/Src/adi_console_menu.o ./Core/Src/adi_console_menu.su ./Core/Src/delay.cyclo ./Core/Src/delay.d ./Core/Src/delay.o ./Core/Src/delay.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/platform_support.cyclo ./Core/Src/platform_support.d ./Core/Src/platform_support.o ./Core/Src/platform_support.su ./Core/Src/spi.cyclo ./Core/Src/spi.d ./Core/Src/spi.o ./Core/Src/spi.su ./Core/Src/stm32g4xx_hal_msp.cyclo ./Core/Src/stm32g4xx_hal_msp.d ./Core/Src/stm32g4xx_hal_msp.o ./Core/Src/stm32g4xx_hal_msp.su ./Core/Src/stm32g4xx_it.cyclo ./Core/Src/stm32g4xx_it.d ./Core/Src/stm32g4xx_it.o ./Core/Src/stm32g4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32g4xx.cyclo ./Core/Src/system_stm32g4xx.d ./Core/Src/system_stm32g4xx.o ./Core/Src/system_stm32g4xx.su

.PHONY: clean-Core-2f-Src


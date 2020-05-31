################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/ksz8851/ksz8851.c 

OBJS += \
./Core/ksz8851/ksz8851.o 

C_DEPS += \
./Core/ksz8851/ksz8851.d 


# Each subdirectory must supply rules for building sources it contributes
Core/ksz8851/ksz8851.o: ../Core/ksz8851/ksz8851.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -DSTM32F091xC -DUSE_HAL_DRIVER -c -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../Core/ksz8851 -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/ksz8851/ksz8851.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"


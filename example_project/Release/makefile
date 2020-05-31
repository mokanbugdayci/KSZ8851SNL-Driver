################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F0xx_HAL_Driver/Src/subdir.mk
-include Core/ksz8851/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
SPI_Tutorial_Master.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
SPI_Tutorial_Master.list \

OBJCOPY_BIN += \
SPI_Tutorial_Master.bin \


# All Target
all: SPI_Tutorial_Master.elf secondary-outputs

# Tool invocations
SPI_Tutorial_Master.elf: $(OBJS) $(USER_OBJS) C:\Users\MOB\Documents\F091_Training\SPI_Tutorial_Master\STM32F091RCTX_FLASH.ld
	arm-none-eabi-gcc -o "SPI_Tutorial_Master.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m0 -T"C:\Users\MOB\Documents\F091_Training\SPI_Tutorial_Master\STM32F091RCTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="SPI_Tutorial_Master.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

SPI_Tutorial_Master.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "SPI_Tutorial_Master.list"
	@echo 'Finished building: $@'
	@echo ' '

SPI_Tutorial_Master.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "SPI_Tutorial_Master.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

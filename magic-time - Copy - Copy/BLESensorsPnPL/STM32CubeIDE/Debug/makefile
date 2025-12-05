################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middlewares/parson/Data\ Exchange/parson/subdir.mk
-include Middlewares/filex/common/src/subdir.mk
-include Middlewares/filex/common/drivers/subdir.mk
-include Middlewares/STM32_BLE_Manager/BLE_Features/PnP/PnPLike/subdir.mk
-include Middlewares/STM32_BLE_Manager/BLE_Features/HardwareSensors/Inertial/subdir.mk
-include Middlewares/STM32_BLE_Manager/BLE_Features/HardwareSensors/Environmental/subdir.mk
-include Middlewares/STM32_BLE_Manager/BLE_Features/HardwareSensors/Battery/subdir.mk
-include Middlewares/STM32_BLE_Manager/BLE_Features/BLE_Manager/subdir.mk
-include Middlewares/PnPLCompManager/Middlewares/PnPLCompManager/subdir.mk
-include Middlewares/BlueNRG-2/Wireless/BlueNRG-2/Utils/subdir.mk
-include Middlewares/BlueNRG-2/Wireless/BlueNRG-2/HCI_TL/Basic/subdir.mk
-include Middlewares/BlueNRG-2/Wireless/BlueNRG-2/Controller/subdir.mk
-include FileX/Target/subdir.mk
-include FileX/App/subdir.mk
-include Drivers/STM32U5xx_HAL_Driver/subdir.mk
-include Drivers/CMSIS/subdir.mk
-include Drivers/BSP/STWIN.box/subdir.mk
-include Drivers/BSP/Components/subdir.mk
-include Application/User/subdir.mk
-include Application/Startup/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := BLESensorsPnPL
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
BLESensorsPnPL.elf \

MAP_FILES += \
BLESensorsPnPL.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
BLESensorsPnPL.list \

OBJCOPY_BIN += \
BLESensorsPnPL.bin \


# All Target
all: main-build

# Main-build Target
main-build: BLESensorsPnPL.elf secondary-outputs

# Tool invocations
BLESensorsPnPL.elf BLESensorsPnPL.map: $(OBJS) $(USER_OBJS) C:\Users\kerem\OneDrive\Documents\magic-time\ -\ Copy\BLESensorsPnPL\STM32CubeIDE\STM32U585AIIXQ_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "BLESensorsPnPL.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m33 -T"C:\Users\kerem\OneDrive\Documents\magic-time - Copy\BLESensorsPnPL\STM32CubeIDE\STM32U585AIIXQ_FLASH.ld" --specs=nosys.specs -Wl,-Map="BLESensorsPnPL.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -u _printf_float -u _scanf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

BLESensorsPnPL.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "BLESensorsPnPL.list"
	@echo 'Finished building: $@'
	@echo ' '

BLESensorsPnPL.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "BLESensorsPnPL.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) BLESensorsPnPL.bin BLESensorsPnPL.elf BLESensorsPnPL.list BLESensorsPnPL.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets

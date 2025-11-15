################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/kerem/OneDrive/Documents/stm32-prototype/BLESensorsPnPL/Middlewares/ST/STM32_BLE_Manager/Src/ble_battery.c 

OBJS += \
./Middlewares/STM32_BLE_Manager/BLE_Features/HardwareSensors/Battery/ble_battery.o 

C_DEPS += \
./Middlewares/STM32_BLE_Manager/BLE_Features/HardwareSensors/Battery/ble_battery.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/STM32_BLE_Manager/BLE_Features/HardwareSensors/Battery/ble_battery.o: C:/Users/kerem/OneDrive/Documents/stm32-prototype/BLESensorsPnPL/Middlewares/ST/STM32_BLE_Manager/Src/ble_battery.c Middlewares/STM32_BLE_Manager/BLE_Features/HardwareSensors/Battery/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U585xx -c -I../../Inc -I../../Drivers/STM32U5xx_HAL_Driver/Inc -I../../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/STM32_BLE_Manager/Inc -I../../Middlewares/Third_Party/parson -I../../Drivers/BSP/STWIN.box -I../../Drivers/BSP/Components/Common -I../../Middlewares/ST/PnPLCompManager/Inc -I../../Middlewares/ST/BlueNRG-2/hci/hci_tl_patterns/Basic -I../../Middlewares/ST/BlueNRG-2/utils -I../../Middlewares/ST/BlueNRG-2/includes -I../../Drivers/BSP/Components/ism330dhcx -I../../Drivers/BSP/Components/iis2mdc -I../../Drivers/BSP/Components/iis2dlpc -I../../Drivers/BSP/Components/iis2iclx -I../../Drivers/BSP/Components/iis3dwb -I../../Drivers/BSP/Components/stts22h -I../../Drivers/BSP/Components/ilps22qs -I../../Drivers/BSP/Components/ST25DV -O0 -ffunction-sections -fdata-sections -Wall -Wno-format -Wno-enum-compare -Wno-enum-conversion -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-STM32_BLE_Manager-2f-BLE_Features-2f-HardwareSensors-2f-Battery

clean-Middlewares-2f-STM32_BLE_Manager-2f-BLE_Features-2f-HardwareSensors-2f-Battery:
	-$(RM) ./Middlewares/STM32_BLE_Manager/BLE_Features/HardwareSensors/Battery/ble_battery.cyclo ./Middlewares/STM32_BLE_Manager/BLE_Features/HardwareSensors/Battery/ble_battery.d ./Middlewares/STM32_BLE_Manager/BLE_Features/HardwareSensors/Battery/ble_battery.o ./Middlewares/STM32_BLE_Manager/BLE_Features/HardwareSensors/Battery/ble_battery.su

.PHONY: clean-Middlewares-2f-STM32_BLE_Manager-2f-BLE_Features-2f-HardwareSensors-2f-Battery


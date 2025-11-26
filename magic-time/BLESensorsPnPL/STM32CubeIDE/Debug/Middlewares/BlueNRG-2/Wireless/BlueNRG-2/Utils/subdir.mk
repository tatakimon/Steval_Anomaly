################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/kerem/OneDrive/Documents/magic-time/BLESensorsPnPL/Middlewares/ST/BlueNRG-2/utils/ble_list.c 

OBJS += \
./Middlewares/BlueNRG-2/Wireless/BlueNRG-2/Utils/ble_list.o 

C_DEPS += \
./Middlewares/BlueNRG-2/Wireless/BlueNRG-2/Utils/ble_list.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/BlueNRG-2/Wireless/BlueNRG-2/Utils/ble_list.o: C:/Users/kerem/OneDrive/Documents/magic-time/BLESensorsPnPL/Middlewares/ST/BlueNRG-2/utils/ble_list.c Middlewares/BlueNRG-2/Wireless/BlueNRG-2/Utils/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U585xx -c -I../../Inc -I../../Drivers/STM32U5xx_HAL_Driver/Inc -I../../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/STM32_BLE_Manager/Inc -I../../Middlewares/Third_Party/parson -I../../Drivers/BSP/STWIN.box -I../../Drivers/BSP/Components/Common -I../../Middlewares/ST/PnPLCompManager/Inc -I../../Middlewares/ST/BlueNRG-2/hci/hci_tl_patterns/Basic -I../../Middlewares/ST/BlueNRG-2/utils -I../../Middlewares/ST/BlueNRG-2/includes -I../../Drivers/BSP/Components/ism330dhcx -I../../Drivers/BSP/Components/iis2mdc -I../../Drivers/BSP/Components/iis2dlpc -I../../Drivers/BSP/Components/iis2iclx -I../../Drivers/BSP/Components/iis3dwb -I../../Drivers/BSP/Components/stts22h -I../../Drivers/BSP/Components/ilps22qs -I../../Drivers/BSP/Components/ST25DV -O0 -ffunction-sections -fdata-sections -Wall -Wno-format -Wno-enum-compare -Wno-enum-conversion -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-BlueNRG-2d-2-2f-Wireless-2f-BlueNRG-2d-2-2f-Utils

clean-Middlewares-2f-BlueNRG-2d-2-2f-Wireless-2f-BlueNRG-2d-2-2f-Utils:
	-$(RM) ./Middlewares/BlueNRG-2/Wireless/BlueNRG-2/Utils/ble_list.cyclo ./Middlewares/BlueNRG-2/Wireless/BlueNRG-2/Utils/ble_list.d ./Middlewares/BlueNRG-2/Wireless/BlueNRG-2/Utils/ble_list.o ./Middlewares/BlueNRG-2/Wireless/BlueNRG-2/Utils/ble_list.su

.PHONY: clean-Middlewares-2f-BlueNRG-2d-2-2f-Wireless-2f-BlueNRG-2d-2-2f-Utils


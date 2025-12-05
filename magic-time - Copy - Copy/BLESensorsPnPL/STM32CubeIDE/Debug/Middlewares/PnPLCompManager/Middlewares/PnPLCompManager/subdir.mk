################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/kerem/OneDrive/Documents/magic-time\ -\ Copy/BLESensorsPnPL/Middlewares/ST/PnPLCompManager/Src/IPnPLComponent.c \
C:/Users/kerem/OneDrive/Documents/magic-time\ -\ Copy/BLESensorsPnPL/Middlewares/ST/PnPLCompManager/Src/PnPLCompManager.c 

OBJS += \
./Middlewares/PnPLCompManager/Middlewares/PnPLCompManager/IPnPLComponent.o \
./Middlewares/PnPLCompManager/Middlewares/PnPLCompManager/PnPLCompManager.o 

C_DEPS += \
./Middlewares/PnPLCompManager/Middlewares/PnPLCompManager/IPnPLComponent.d \
./Middlewares/PnPLCompManager/Middlewares/PnPLCompManager/PnPLCompManager.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/PnPLCompManager/Middlewares/PnPLCompManager/IPnPLComponent.o: C:/Users/kerem/OneDrive/Documents/magic-time\ -\ Copy/BLESensorsPnPL/Middlewares/ST/PnPLCompManager/Src/IPnPLComponent.c Middlewares/PnPLCompManager/Middlewares/PnPLCompManager/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DFX_STANDALONE_ENABLE -DDEBUG -DUSE_HAL_DRIVER -DSTM32U585xx -c -I"C:/Users/kerem/OneDrive/Documents/magic-time - Copy/BLESensorsPnPL/Middlewares/ST/filex/common/inc" -I"C:/Users/kerem/OneDrive/Documents/magic-time - Copy/BLESensorsPnPL/Middlewares/ST/filex/ports/generic/inc" -I../../Inc -I../../Drivers/STM32U5xx_HAL_Driver/Inc -I../../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/STM32_BLE_Manager/Inc -I../../Middlewares/Third_Party/parson -I../../Drivers/BSP/STWIN.box -I../../Drivers/BSP/Components/Common -I../../Middlewares/ST/PnPLCompManager/Inc -I../../Middlewares/ST/BlueNRG-2/hci/hci_tl_patterns/Basic -I../../Middlewares/ST/BlueNRG-2/utils -I../../Middlewares/ST/BlueNRG-2/includes -I../../Drivers/BSP/Components/ism330dhcx -I../../Drivers/BSP/Components/iis2mdc -I../../Drivers/BSP/Components/iis2dlpc -I../../Drivers/BSP/Components/iis2iclx -I../../Drivers/BSP/Components/iis3dwb -I../../Drivers/BSP/Components/stts22h -I../../Drivers/BSP/Components/ilps22qs -I../../Drivers/BSP/Components/ST25DV -I"C:/Users/kerem/OneDrive/Documents/magic-time - Copy/BLESensorsPnPL/STM32CubeIDE/FileX/Target" -I"C:/Users/kerem/OneDrive/Documents/magic-time - Copy/BLESensorsPnPL/STM32CubeIDE/FileX/App" -O0 -ffunction-sections -fdata-sections -Wall -Wno-format -Wno-enum-compare -Wno-enum-conversion -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"Middlewares/PnPLCompManager/Middlewares/PnPLCompManager/IPnPLComponent.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/PnPLCompManager/Middlewares/PnPLCompManager/PnPLCompManager.o: C:/Users/kerem/OneDrive/Documents/magic-time\ -\ Copy/BLESensorsPnPL/Middlewares/ST/PnPLCompManager/Src/PnPLCompManager.c Middlewares/PnPLCompManager/Middlewares/PnPLCompManager/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DFX_STANDALONE_ENABLE -DDEBUG -DUSE_HAL_DRIVER -DSTM32U585xx -c -I"C:/Users/kerem/OneDrive/Documents/magic-time - Copy/BLESensorsPnPL/Middlewares/ST/filex/common/inc" -I"C:/Users/kerem/OneDrive/Documents/magic-time - Copy/BLESensorsPnPL/Middlewares/ST/filex/ports/generic/inc" -I../../Inc -I../../Drivers/STM32U5xx_HAL_Driver/Inc -I../../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/STM32_BLE_Manager/Inc -I../../Middlewares/Third_Party/parson -I../../Drivers/BSP/STWIN.box -I../../Drivers/BSP/Components/Common -I../../Middlewares/ST/PnPLCompManager/Inc -I../../Middlewares/ST/BlueNRG-2/hci/hci_tl_patterns/Basic -I../../Middlewares/ST/BlueNRG-2/utils -I../../Middlewares/ST/BlueNRG-2/includes -I../../Drivers/BSP/Components/ism330dhcx -I../../Drivers/BSP/Components/iis2mdc -I../../Drivers/BSP/Components/iis2dlpc -I../../Drivers/BSP/Components/iis2iclx -I../../Drivers/BSP/Components/iis3dwb -I../../Drivers/BSP/Components/stts22h -I../../Drivers/BSP/Components/ilps22qs -I../../Drivers/BSP/Components/ST25DV -I"C:/Users/kerem/OneDrive/Documents/magic-time - Copy/BLESensorsPnPL/STM32CubeIDE/FileX/Target" -I"C:/Users/kerem/OneDrive/Documents/magic-time - Copy/BLESensorsPnPL/STM32CubeIDE/FileX/App" -O0 -ffunction-sections -fdata-sections -Wall -Wno-format -Wno-enum-compare -Wno-enum-conversion -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"Middlewares/PnPLCompManager/Middlewares/PnPLCompManager/PnPLCompManager.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-PnPLCompManager-2f-Middlewares-2f-PnPLCompManager

clean-Middlewares-2f-PnPLCompManager-2f-Middlewares-2f-PnPLCompManager:
	-$(RM) ./Middlewares/PnPLCompManager/Middlewares/PnPLCompManager/IPnPLComponent.cyclo ./Middlewares/PnPLCompManager/Middlewares/PnPLCompManager/IPnPLComponent.d ./Middlewares/PnPLCompManager/Middlewares/PnPLCompManager/IPnPLComponent.o ./Middlewares/PnPLCompManager/Middlewares/PnPLCompManager/IPnPLComponent.su ./Middlewares/PnPLCompManager/Middlewares/PnPLCompManager/PnPLCompManager.cyclo ./Middlewares/PnPLCompManager/Middlewares/PnPLCompManager/PnPLCompManager.d ./Middlewares/PnPLCompManager/Middlewares/PnPLCompManager/PnPLCompManager.o ./Middlewares/PnPLCompManager/Middlewares/PnPLCompManager/PnPLCompManager.su

.PHONY: clean-Middlewares-2f-PnPLCompManager-2f-Middlewares-2f-PnPLCompManager


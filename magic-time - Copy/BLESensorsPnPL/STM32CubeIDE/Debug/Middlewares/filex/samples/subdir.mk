################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/filex/samples/demo_filex.c 

OBJS += \
./Middlewares/filex/samples/demo_filex.o 

C_DEPS += \
./Middlewares/filex/samples/demo_filex.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/filex/samples/%.o Middlewares/filex/samples/%.su Middlewares/filex/samples/%.cyclo: ../Middlewares/filex/samples/%.c Middlewares/filex/samples/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U585xx -c -I../../Inc -I../../Drivers/STM32U5xx_HAL_Driver/Inc -I../../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/STM32_BLE_Manager/Inc -I../../Middlewares/Third_Party/parson -I../../Drivers/BSP/STWIN.box -I../../Drivers/BSP/Components/Common -I../../Middlewares/ST/PnPLCompManager/Inc -I../../Middlewares/ST/BlueNRG-2/hci/hci_tl_patterns/Basic -I../../Middlewares/ST/BlueNRG-2/utils -I../../Middlewares/ST/BlueNRG-2/includes -I../../Drivers/BSP/Components/ism330dhcx -I../../Drivers/BSP/Components/iis2mdc -I../../Drivers/BSP/Components/iis2dlpc -I../../Drivers/BSP/Components/iis2iclx -I../../Drivers/BSP/Components/iis3dwb -I../../Drivers/BSP/Components/stts22h -I../../Drivers/BSP/Components/ilps22qs -I../../Drivers/BSP/Components/ST25DV -I"C:/Users/kerem/OneDrive/Documents/magic-time - Copy/BLESensorsPnPL/Middlewares/ST/filex" -I"C:/Users/kerem/OneDrive/Documents/magic-time - Copy/BLESensorsPnPL/STM32CubeIDE/Middlewares/threadx/ports" -I"C:/Users/kerem/OneDrive/Documents/magic-time - Copy/BLESensorsPnPL/STM32CubeIDE/FileX/Target" -O0 -ffunction-sections -fdata-sections -Wall -Wno-format -Wno-enum-compare -Wno-enum-conversion -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-filex-2f-samples

clean-Middlewares-2f-filex-2f-samples:
	-$(RM) ./Middlewares/filex/samples/demo_filex.cyclo ./Middlewares/filex/samples/demo_filex.d ./Middlewares/filex/samples/demo_filex.o ./Middlewares/filex/samples/demo_filex.su

.PHONY: clean-Middlewares-2f-filex-2f-samples


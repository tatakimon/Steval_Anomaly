################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/filex/common/drivers/template/fx_stm32_driver.c \
../Middlewares/filex/common/drivers/template/fx_stm32_mmc_driver_glue.c \
../Middlewares/filex/common/drivers/template/fx_stm32_sd_driver_glue.c \
../Middlewares/filex/common/drivers/template/fx_stm32_sd_driver_glue_dma_rtos.c \
../Middlewares/filex/common/drivers/template/fx_stm32_sd_driver_glue_dma_standalone.c 

OBJS += \
./Middlewares/filex/common/drivers/template/fx_stm32_driver.o \
./Middlewares/filex/common/drivers/template/fx_stm32_mmc_driver_glue.o \
./Middlewares/filex/common/drivers/template/fx_stm32_sd_driver_glue.o \
./Middlewares/filex/common/drivers/template/fx_stm32_sd_driver_glue_dma_rtos.o \
./Middlewares/filex/common/drivers/template/fx_stm32_sd_driver_glue_dma_standalone.o 

C_DEPS += \
./Middlewares/filex/common/drivers/template/fx_stm32_driver.d \
./Middlewares/filex/common/drivers/template/fx_stm32_mmc_driver_glue.d \
./Middlewares/filex/common/drivers/template/fx_stm32_sd_driver_glue.d \
./Middlewares/filex/common/drivers/template/fx_stm32_sd_driver_glue_dma_rtos.d \
./Middlewares/filex/common/drivers/template/fx_stm32_sd_driver_glue_dma_standalone.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/filex/common/drivers/template/%.o Middlewares/filex/common/drivers/template/%.su Middlewares/filex/common/drivers/template/%.cyclo: ../Middlewares/filex/common/drivers/template/%.c Middlewares/filex/common/drivers/template/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DFX_STANDALONE_ENABLE -DDEBUG -DUSE_HAL_DRIVER -DSTM32U585xx -c -I"C:/Users/kerem/OneDrive/Documents/magic-time - Copy/BLESensorsPnPL/Middlewares/ST/filex/common/inc" -I"C:/Users/kerem/OneDrive/Documents/magic-time - Copy/BLESensorsPnPL/Middlewares/ST/filex/ports/generic/inc" -I../../Inc -I../../Drivers/STM32U5xx_HAL_Driver/Inc -I../../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/STM32_BLE_Manager/Inc -I../../Middlewares/Third_Party/parson -I../../Drivers/BSP/STWIN.box -I../../Drivers/BSP/Components/Common -I../../Middlewares/ST/PnPLCompManager/Inc -I../../Middlewares/ST/BlueNRG-2/hci/hci_tl_patterns/Basic -I../../Middlewares/ST/BlueNRG-2/utils -I../../Middlewares/ST/BlueNRG-2/includes -I../../Drivers/BSP/Components/ism330dhcx -I../../Drivers/BSP/Components/iis2mdc -I../../Drivers/BSP/Components/iis2dlpc -I../../Drivers/BSP/Components/iis2iclx -I../../Drivers/BSP/Components/iis3dwb -I../../Drivers/BSP/Components/stts22h -I../../Drivers/BSP/Components/ilps22qs -I../../Drivers/BSP/Components/ST25DV -I"C:/Users/kerem/OneDrive/Documents/magic-time - Copy/BLESensorsPnPL/STM32CubeIDE/FileX/Target" -I"C:/Users/kerem/OneDrive/Documents/magic-time - Copy/BLESensorsPnPL/STM32CubeIDE/FileX/App" -O0 -ffunction-sections -fdata-sections -Wall -Wno-format -Wno-enum-compare -Wno-enum-conversion -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-filex-2f-common-2f-drivers-2f-template

clean-Middlewares-2f-filex-2f-common-2f-drivers-2f-template:
	-$(RM) ./Middlewares/filex/common/drivers/template/fx_stm32_driver.cyclo ./Middlewares/filex/common/drivers/template/fx_stm32_driver.d ./Middlewares/filex/common/drivers/template/fx_stm32_driver.o ./Middlewares/filex/common/drivers/template/fx_stm32_driver.su ./Middlewares/filex/common/drivers/template/fx_stm32_mmc_driver_glue.cyclo ./Middlewares/filex/common/drivers/template/fx_stm32_mmc_driver_glue.d ./Middlewares/filex/common/drivers/template/fx_stm32_mmc_driver_glue.o ./Middlewares/filex/common/drivers/template/fx_stm32_mmc_driver_glue.su ./Middlewares/filex/common/drivers/template/fx_stm32_sd_driver_glue.cyclo ./Middlewares/filex/common/drivers/template/fx_stm32_sd_driver_glue.d ./Middlewares/filex/common/drivers/template/fx_stm32_sd_driver_glue.o ./Middlewares/filex/common/drivers/template/fx_stm32_sd_driver_glue.su ./Middlewares/filex/common/drivers/template/fx_stm32_sd_driver_glue_dma_rtos.cyclo ./Middlewares/filex/common/drivers/template/fx_stm32_sd_driver_glue_dma_rtos.d ./Middlewares/filex/common/drivers/template/fx_stm32_sd_driver_glue_dma_rtos.o ./Middlewares/filex/common/drivers/template/fx_stm32_sd_driver_glue_dma_rtos.su ./Middlewares/filex/common/drivers/template/fx_stm32_sd_driver_glue_dma_standalone.cyclo ./Middlewares/filex/common/drivers/template/fx_stm32_sd_driver_glue_dma_standalone.d ./Middlewares/filex/common/drivers/template/fx_stm32_sd_driver_glue_dma_standalone.o ./Middlewares/filex/common/drivers/template/fx_stm32_sd_driver_glue_dma_standalone.su

.PHONY: clean-Middlewares-2f-filex-2f-common-2f-drivers-2f-template


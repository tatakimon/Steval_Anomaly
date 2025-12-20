################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Components/stc3115/STC3115_Driver.c 

OBJS += \
./Drivers/Components/stc3115/STC3115_Driver.o 

C_DEPS += \
./Drivers/Components/stc3115/STC3115_Driver.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Components/stc3115/%.o Drivers/Components/stc3115/%.su Drivers/Components/stc3115/%.cyclo: ../Drivers/Components/stc3115/%.c Drivers/Components/stc3115/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U585xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Components-2f-stc3115

clean-Drivers-2f-Components-2f-stc3115:
	-$(RM) ./Drivers/Components/stc3115/STC3115_Driver.cyclo ./Drivers/Components/stc3115/STC3115_Driver.d ./Drivers/Components/stc3115/STC3115_Driver.o ./Drivers/Components/stc3115/STC3115_Driver.su

.PHONY: clean-Drivers-2f-Components-2f-stc3115


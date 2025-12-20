################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Components/iis3dwb/iis3dwb.c \
../Drivers/Components/iis3dwb/iis3dwb_reg.c 

OBJS += \
./Drivers/Components/iis3dwb/iis3dwb.o \
./Drivers/Components/iis3dwb/iis3dwb_reg.o 

C_DEPS += \
./Drivers/Components/iis3dwb/iis3dwb.d \
./Drivers/Components/iis3dwb/iis3dwb_reg.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Components/iis3dwb/%.o Drivers/Components/iis3dwb/%.su Drivers/Components/iis3dwb/%.cyclo: ../Drivers/Components/iis3dwb/%.c Drivers/Components/iis3dwb/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U585xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Components-2f-iis3dwb

clean-Drivers-2f-Components-2f-iis3dwb:
	-$(RM) ./Drivers/Components/iis3dwb/iis3dwb.cyclo ./Drivers/Components/iis3dwb/iis3dwb.d ./Drivers/Components/iis3dwb/iis3dwb.o ./Drivers/Components/iis3dwb/iis3dwb.su ./Drivers/Components/iis3dwb/iis3dwb_reg.cyclo ./Drivers/Components/iis3dwb/iis3dwb_reg.d ./Drivers/Components/iis3dwb/iis3dwb_reg.o ./Drivers/Components/iis3dwb/iis3dwb_reg.su

.PHONY: clean-Drivers-2f-Components-2f-iis3dwb


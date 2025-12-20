################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Components/ilps22qs/ilps22qs.c \
../Drivers/Components/ilps22qs/ilps22qs_reg.c 

OBJS += \
./Drivers/Components/ilps22qs/ilps22qs.o \
./Drivers/Components/ilps22qs/ilps22qs_reg.o 

C_DEPS += \
./Drivers/Components/ilps22qs/ilps22qs.d \
./Drivers/Components/ilps22qs/ilps22qs_reg.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Components/ilps22qs/%.o Drivers/Components/ilps22qs/%.su Drivers/Components/ilps22qs/%.cyclo: ../Drivers/Components/ilps22qs/%.c Drivers/Components/ilps22qs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U585xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Components-2f-ilps22qs

clean-Drivers-2f-Components-2f-ilps22qs:
	-$(RM) ./Drivers/Components/ilps22qs/ilps22qs.cyclo ./Drivers/Components/ilps22qs/ilps22qs.d ./Drivers/Components/ilps22qs/ilps22qs.o ./Drivers/Components/ilps22qs/ilps22qs.su ./Drivers/Components/ilps22qs/ilps22qs_reg.cyclo ./Drivers/Components/ilps22qs/ilps22qs_reg.d ./Drivers/Components/ilps22qs/ilps22qs_reg.o ./Drivers/Components/ilps22qs/ilps22qs_reg.su

.PHONY: clean-Drivers-2f-Components-2f-ilps22qs


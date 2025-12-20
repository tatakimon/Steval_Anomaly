################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Components/ST25DV/st25dv.c \
../Drivers/Components/ST25DV/st25dv_reg.c 

OBJS += \
./Drivers/Components/ST25DV/st25dv.o \
./Drivers/Components/ST25DV/st25dv_reg.o 

C_DEPS += \
./Drivers/Components/ST25DV/st25dv.d \
./Drivers/Components/ST25DV/st25dv_reg.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Components/ST25DV/%.o Drivers/Components/ST25DV/%.su Drivers/Components/ST25DV/%.cyclo: ../Drivers/Components/ST25DV/%.c Drivers/Components/ST25DV/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U585xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Components-2f-ST25DV

clean-Drivers-2f-Components-2f-ST25DV:
	-$(RM) ./Drivers/Components/ST25DV/st25dv.cyclo ./Drivers/Components/ST25DV/st25dv.d ./Drivers/Components/ST25DV/st25dv.o ./Drivers/Components/ST25DV/st25dv.su ./Drivers/Components/ST25DV/st25dv_reg.cyclo ./Drivers/Components/ST25DV/st25dv_reg.d ./Drivers/Components/ST25DV/st25dv_reg.o ./Drivers/Components/ST25DV/st25dv_reg.su

.PHONY: clean-Drivers-2f-Components-2f-ST25DV


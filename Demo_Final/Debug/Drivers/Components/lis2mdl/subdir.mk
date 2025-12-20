################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Components/lis2mdl/lis2mdl.c \
../Drivers/Components/lis2mdl/lis2mdl_reg.c 

OBJS += \
./Drivers/Components/lis2mdl/lis2mdl.o \
./Drivers/Components/lis2mdl/lis2mdl_reg.o 

C_DEPS += \
./Drivers/Components/lis2mdl/lis2mdl.d \
./Drivers/Components/lis2mdl/lis2mdl_reg.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Components/lis2mdl/%.o Drivers/Components/lis2mdl/%.su Drivers/Components/lis2mdl/%.cyclo: ../Drivers/Components/lis2mdl/%.c Drivers/Components/lis2mdl/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U585xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Components-2f-lis2mdl

clean-Drivers-2f-Components-2f-lis2mdl:
	-$(RM) ./Drivers/Components/lis2mdl/lis2mdl.cyclo ./Drivers/Components/lis2mdl/lis2mdl.d ./Drivers/Components/lis2mdl/lis2mdl.o ./Drivers/Components/lis2mdl/lis2mdl.su ./Drivers/Components/lis2mdl/lis2mdl_reg.cyclo ./Drivers/Components/lis2mdl/lis2mdl_reg.d ./Drivers/Components/lis2mdl/lis2mdl_reg.o ./Drivers/Components/lis2mdl/lis2mdl_reg.su

.PHONY: clean-Drivers-2f-Components-2f-lis2mdl


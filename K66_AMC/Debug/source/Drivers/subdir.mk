################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/Drivers/LED_RGB.c \
../source/Drivers/PWM.c \
../source/Drivers/UART.c 

C_DEPS += \
./source/Drivers/LED_RGB.d \
./source/Drivers/PWM.d \
./source/Drivers/UART.d 

OBJS += \
./source/Drivers/LED_RGB.o \
./source/Drivers/PWM.o \
./source/Drivers/UART.o 


# Each subdirectory must supply rules for building sources it contributes
source/Drivers/%.o: ../source/Drivers/%.c source/Drivers/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MK66FN2M0VMD18 -DCPU_MK66FN2M0VMD18_cm4 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DSDK_OS_FREE_RTOS -I"/home/kenneth/Robotica/AUTOMODELCAR/board" -I"/home/kenneth/Robotica/AUTOMODELCAR/drivers" -I"/home/kenneth/Robotica/AUTOMODELCAR/component/uart" -I"/home/kenneth/Robotica/AUTOMODELCAR/device" -I"/home/kenneth/Robotica/AUTOMODELCAR/CMSIS" -I"/home/kenneth/Robotica/AUTOMODELCAR/component/serial_manager" -I"/home/kenneth/Robotica/AUTOMODELCAR/component/lists" -I"/home/kenneth/Robotica/AUTOMODELCAR/utilities" -I"/home/kenneth/Robotica/AUTOMODELCAR/freertos/freertos_kernel/include" -I"/home/kenneth/Robotica/AUTOMODELCAR/freertos/freertos_kernel/portable/GCC/ARM_CM4F" -I"/home/kenneth/Robotica/AUTOMODELCAR/source" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source-2f-Drivers

clean-source-2f-Drivers:
	-$(RM) ./source/Drivers/LED_RGB.d ./source/Drivers/LED_RGB.o ./source/Drivers/PWM.d ./source/Drivers/PWM.o ./source/Drivers/UART.d ./source/Drivers/UART.o

.PHONY: clean-source-2f-Drivers


################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/Drivers/I2C.c \
../source/Drivers/LED_RGB.c \
../source/Drivers/PWM.c \
../source/Drivers/UART.c \
../source/Drivers/bno055.c 

C_DEPS += \
./source/Drivers/I2C.d \
./source/Drivers/LED_RGB.d \
./source/Drivers/PWM.d \
./source/Drivers/UART.d \
./source/Drivers/bno055.d 

OBJS += \
./source/Drivers/I2C.o \
./source/Drivers/LED_RGB.o \
./source/Drivers/PWM.o \
./source/Drivers/UART.o \
./source/Drivers/bno055.o 


# Each subdirectory must supply rules for building sources it contributes
source/Drivers/%.o: ../source/Drivers/%.c source/Drivers/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MK66FN2M0VMD18 -DCPU_MK66FN2M0VMD18_cm4 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DSDK_OS_FREE_RTOS -I"/home/kenneth/Robotica/AUTOMODELCAR/K66_AMC/board" -I"/home/kenneth/Robotica/AUTOMODELCAR/K66_AMC/drivers" -I"/home/kenneth/Robotica/AUTOMODELCAR/K66_AMC/component/uart" -I"/home/kenneth/Robotica/AUTOMODELCAR/K66_AMC/device" -I"/home/kenneth/Robotica/AUTOMODELCAR/K66_AMC/CMSIS" -I"/home/kenneth/Robotica/AUTOMODELCAR/K66_AMC/component/serial_manager" -I"/home/kenneth/Robotica/AUTOMODELCAR/K66_AMC/component/lists" -I"/home/kenneth/Robotica/AUTOMODELCAR/K66_AMC/utilities" -I"/home/kenneth/Robotica/AUTOMODELCAR/K66_AMC/CMSIS/DSP/Include" -I"/home/kenneth/Robotica/AUTOMODELCAR/K66_AMC/freertos/freertos_kernel/include" -I"/home/kenneth/Robotica/AUTOMODELCAR/K66_AMC/freertos/freertos_kernel/portable/GCC/ARM_CM4F" -I"/home/kenneth/Robotica/AUTOMODELCAR/K66_AMC/source" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source-2f-Drivers

clean-source-2f-Drivers:
	-$(RM) ./source/Drivers/I2C.d ./source/Drivers/I2C.o ./source/Drivers/LED_RGB.d ./source/Drivers/LED_RGB.o ./source/Drivers/PWM.d ./source/Drivers/PWM.o ./source/Drivers/UART.d ./source/Drivers/UART.o ./source/Drivers/bno055.d ./source/Drivers/bno055.o

.PHONY: clean-source-2f-Drivers


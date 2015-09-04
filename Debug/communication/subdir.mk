################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../communication/aiox.c \
../communication/data.c 

OBJS += \
./communication/aiox.o \
./communication/data.o 

C_DEPS += \
./communication/aiox.d \
./communication/data.d 


# Each subdirectory must supply rules for building sources it contributes
communication/%.o: ../communication/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



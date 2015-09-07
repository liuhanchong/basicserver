################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../logmanage/error.c \
../logmanage/log.c 

OBJS += \
./logmanage/error.o \
./logmanage/log.o 

C_DEPS += \
./logmanage/error.d \
./logmanage/log.d 


# Each subdirectory must supply rules for building sources it contributes
logmanage/%.o: ../logmanage/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



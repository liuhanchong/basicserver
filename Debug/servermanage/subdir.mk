################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../servermanage/servermanage.c 

OBJS += \
./servermanage/servermanage.o 

C_DEPS += \
./servermanage/servermanage.d 


# Each subdirectory must supply rules for building sources it contributes
servermanage/%.o: ../servermanage/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



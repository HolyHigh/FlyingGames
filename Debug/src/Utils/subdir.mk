################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Utils/LinkedList.c \
../src/Utils/cJSON.c 

OBJS += \
./src/Utils/LinkedList.o \
./src/Utils/cJSON.o 

C_DEPS += \
./src/Utils/LinkedList.d \
./src/Utils/cJSON.d 


# Each subdirectory must supply rules for building sources it contributes
src/Utils/%.o: ../src/Utils/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



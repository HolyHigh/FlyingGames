################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/GeneralAgent.c \
../src/Global.c \
../src/GroupAgent.c \
../src/MajorAgent.c \
../src/Network.c \
../src/main.c 

OBJS += \
./src/GeneralAgent.o \
./src/Global.o \
./src/GroupAgent.o \
./src/MajorAgent.o \
./src/Network.o \
./src/main.o 

C_DEPS += \
./src/GeneralAgent.d \
./src/Global.d \
./src/GroupAgent.d \
./src/MajorAgent.d \
./src/Network.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/filectrl.c \
../source/protocol.c \
../source/raspberryClient.c \
../source/serial.c 

OBJS += \
./source/filectrl.o \
./source/protocol.o \
./source/raspberryClient.o \
./source/serial.o 

C_DEPS += \
./source/filectrl.d \
./source/protocol.d \
./source/raspberryClient.d \
./source/serial.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../execute.c \
../list.c \
../mish.c \
../parser.c 

O_SRCS += \
../execute.o \
../list.o \
../mish.o \
../parser.o 

OBJS += \
./execute.o \
./list.o \
./mish.o \
./parser.o 

C_DEPS += \
./execute.d \
./list.d \
./mish.d \
./parser.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



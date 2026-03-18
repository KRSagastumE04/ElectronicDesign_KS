################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/Parcial2_SD.c \
../Src/game.c \
../Src/lcd.c \
../Src/syscalls.c \
../Src/sysmem.c 

OBJS += \
./Src/Parcial2_SD.o \
./Src/game.o \
./Src/lcd.o \
./Src/syscalls.o \
./Src/sysmem.o 

C_DEPS += \
./Src/Parcial2_SD.d \
./Src/game.d \
./Src/lcd.d \
./Src/syscalls.d \
./Src/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32L0 -DSTM32L053R8Tx -c -I../Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/Parcial2_SD.cyclo ./Src/Parcial2_SD.d ./Src/Parcial2_SD.o ./Src/Parcial2_SD.su ./Src/game.cyclo ./Src/game.d ./Src/game.o ./Src/game.su ./Src/lcd.cyclo ./Src/lcd.d ./Src/lcd.o ./Src/lcd.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su

.PHONY: clean-Src


vpath %.c ../Libraries/STM32F4xx_StdPeriph_Driver/src/ \
../Utilities/STM32F4-Discovery/

vpath %.h ../Libraries/CMSIS/Include/ \
../Libraries/CMSIS/STM32F4xx/ \
../Libraries/STM32F4xx_StdPeriph_Driver/inc/ \
../Utilities/STM32F4-Discovery/

CC	 	 = arm-none-eabi-gcc
OBJCOPY		 = arm-none-eabi-objcopy
SIZE		 = arm-none-eabi-size

DEFS = -DUSE_STDPERIPH_DRIVER -DSTM32F4XX

INCS +=		-I./ \
		-I../Libraries/CMSIS/STM32F4xx/ \
		-I../Utilities/STM32F4-Discovery/ \
		-I../Libraries/STM32F4xx_StdPeriph_Driver/inc/ \
		-I../Libraries/CMSIS/Include/

OBJS += startup_stm32f4xx.o
OBJS += main.o stm32f4xx_it.o system_stm32f4xx.o
OBJS += ../Utilities/STM32F4-Discovery/stm32f4_discovery.o \
	../Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.o \
	../Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.o \
	../Libraries/STM32F4xx_StdPeriph_Driver/src/misc.o \
	../Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.o \
	../Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.o \
	../Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_pwr.o

DEBUG_FLAGS	?= -ggdb3
FP_FLAGS	?= -mfloat-abi=hard -mfpu=fpv4-sp-d16

FLAGS		= -std=gnu99 \
		-Os \
		-g \
		-Wundef \
		-Wall \
		-fno-builtin \
		-ffunction-sections \
		-nostartfiles \
		-lnosys \
		-Wl,-gc-sections \
		-Werror
# -mfpu=fpv5-sp-d16 $(FP_FLAGS)
FLAGS += -mthumb -mcpu=cortex-m4 -Wall $(DEBUG_FLAGS)
LFLAGS += -mthumb -mcpu=cortex-m4

all:IO_Toggle.bin

IO_Toggle.bin:IO_Toggle.elf
	$(OBJCOPY) -O binary -S $< $@

IO_Toggle.elf:$(OBJS)
	$(CC) $(LFALGS) $^ -Tstm32_flash.ld -o $@
	$(SIZE) $@

burn:IO_Toggle.bin
	@st-flash write $< 0x08000000

%.o:%.S
	$(CC) $(FLAGS) -c $< -o $@
%.o:%.c
	$(CC) $(FLAGS) $(DEFS) $(INCS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJS) IO_Toggle.*


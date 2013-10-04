TARGET = ddk-arm

#### Setup ####
TOOLCHAIN	= arm-none-eabi

CC		= $(TOOLCHAIN)-gcc
CP		= $(TOOLCHAIN)-objcopy
SIZE	= $(TOOLCHAIN)-size
CHECKSUM= ./checksum

CFLAGS	+= -Os -ffunction-sections -fdata-sections 
CFLAGS  += -mthumb -mcpu=cortex-m3 -std=c99 -I./
CFLAGS  += -IBase/Core/CM3/DeviceSupport/NXP/LPC17xx
CFLAGS  += -IBase/Core/CM3/CoreSupport
CFLAGS  += -IBase/FreeRTOSV7.3.0/FreeRTOS/Source/include
CFLAGS  += -IBase/FreeRTOSV7.3.0/FreeRTOS/Source/portable/GCC/ARM_CM3
CFLAGS  += -IBase/CDL/Drivers/include
CFLAGS  += -IBase/directc
CFLAGS  += -D RAM_MODE=1
LFLAGS	= -Wl,--gc-sections -Tlpc17xx.ld
CPFLAGS	= -Obinary
CRT0    = ./Base/Core/CM3/DeviceSupport/NXP/LPC17xx/startup/gcc/startup_LPC17xx.S

SRC = \
buffer.c \
helper.c \
io.c \
led.c \
main.c \
stats.c \
uart.c \
tests.c \
jtag_1149.c \
cli.c \
syscalls.c \
Base/Core/CM3/DeviceSupport/NXP/LPC17xx/system_LPC17xx.c \
Base/CDL/Drivers/source/lpc17xx_clkpwr.c \
Base/CDL/Drivers/source/lpc17xx_exti.c \
Base/CDL/Drivers/source/lpc17xx_gpio.c \
Base/CDL/Drivers/source/lpc17xx_libcfg_default.c \
Base/CDL/Drivers/source/lpc17xx_nvic.c \
Base/CDL/Drivers/source/lpc17xx_pinsel.c \
Base/CDL/Drivers/source/lpc17xx_systick.c \
Base/CDL/Drivers/source/lpc17xx_timer.c \
Base/CDL/Drivers/source/lpc17xx_uart.c \
Base/FreeRTOSV7.3.0/FreeRTOS/Source/croutine.c \
Base/FreeRTOSV7.3.0/FreeRTOS/Source/list.c \
Base/FreeRTOSV7.3.0/FreeRTOS/Source/queue.c \
Base/FreeRTOSV7.3.0/FreeRTOS/Source/tasks.c \
Base/FreeRTOSV7.3.0/FreeRTOS/Source/timers.c \
Base/FreeRTOSV7.3.0/FreeRTOS/Source/portable/MemMang/heap_4.c \
Base/FreeRTOSV7.3.0/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c \
Base/directc/dpalg.c \
Base/directc/dpchain.c \
Base/directc/dpcom.c \
Base/directc/dpcore.c \
Base/directc/dpfrom.c \
Base/directc/dpjtag.c \
Base/directc/dpnvm.c \
Base/directc/dpsecurity.c \
Base/directc/dpuser.c \
Base/directc/dputil.c \
bitstream/bitstream.c 

OBJ = $(SRC:.c=.o)


.PHONY: clean install

#### Rules ####
all: $(TARGET).bin

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(TARGET).elf: $(OBJ) $(CRT0)
	$(CC) $(LFLAGS) $(CFLAGS) $^ -o $@

$(TARGET).bin: $(TARGET).elf $(CHECKSUM)
	$(CP) $(CPFLAGS) $< $@
	$(CHECKSUM) $@
	$(SIZE) $<

$(CHECKSUM): checksum.c
	gcc -o $@ $<

clean:
	rm -f $(OBJ) $(CHECKSUM) $(TARGET).elf $(TARGET).bin


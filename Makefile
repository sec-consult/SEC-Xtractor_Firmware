SELF_TEST=0

CCFLAGS_GENERAL=

CC_AVR=avr-gcc
CCFLAGS_AVR=-mmcu=atxmega128a1 -Os -std=gnu99 -DXTRACTOR_ARCH_AVR -DF_CPU=32000000 -DSELF_TEST=$(SELF_TEST)

# CC_TEST=gcc
# CCFLAGS_TEST=-Os -std=gnu99 -DXTRACTOR_ARCH_TEST

SRC_FILES=*.c *.h modules/*.c
SRC_FILES_AVR=$(SRC_FILES) hal/avr/*.c hal/avr/*.h
SRC_FILES_TEST=$(SRC_FILES) hal-test/*.c hal-test/*.h

OBJECTS_AVR=*.o

LDFLAGS_AVR=-mmcu=atxmega128a1 -g

PLATFORM_AVR=x128a1

SERIAL_PORT=/dev/ttyUSB3
BAUD=4000000

AVRDUDE_USB=avrdude -p $(PLATFORM_AVR) -P usb -c atmelice_pdi 
AVRDUDE_SERIAL=avrdude -p $(PLATFORM_AVR) -P $(SERIAL_PORT) -b 115200 -c avr911

# fuses
FUSE0=0xFF
FUSE1=0x00
# BOOTRST set
FUSE2=0x9F
FUSE4=0xFE
FUSE5=0xFF

################################################################################
## Test for Flags                                                             ##
## -e	Enable ANSI Escape Sequence Support                                   ##
################################################################################
consume_flags:
ifneq ($(findstring e, $(MAKEFLAGS)),)
	CCFLAGS_GENERAL+= -DESCAPE_CODE_SUPPORTED
endif

################################################################################
## GENERAL/MISC                                                               ##
################################################################################


avr: main.hex

# test: main-test
# 	./main-test


################################################################################
## AVR DEVICE                                                                 ##
################################################################################

main.hex: main.bin
	avr-objcopy -j .text -j .data -O ihex $^ $@

main.bin: consume_flags $(SRC_FILES_AVR)
	$(CC_AVR) $(CCFLAGS_AVR) main.c -o $@

main.o: consume_flags $(SRC_FILES_AVR)
	$(CC_AVR) $(CCFLAGS_AVR) -c main.c -o $@

# hal-avr: consume_flags $(SRC_FILES_AVR)
# 	$(CC_AVR) $(CCFLAGS_AVR) -c $(SRC_FILES_AVR) -o $^

# main.elf: $(OBJECTS)
# 	$(CC_AVR) $(LDFLAGS) -o $@ $^

bootloader.bin: bootloader.c
	$(CC_AVR) $(CCFLAGS_AVR) bootloader.c -o $@

# bootloader
bootloader.compiled: Xmega_Bootloader/*
	cd Xmega_Bootloader && touch Xmega_Bootloader.c && make
	touch $@

# flashing
programmer-alive:
	$(AVRDUDE_USB)

reset-device:
	echo "!!!!! NOW PRESS AND HOLD THE BUTTON LABELED 'BOOT' TO ENTER BOOTLOADER !!!"
	sleep 2
	python reset_device.py $(SERIAL_PORT) $(BAUD)
	# wait for bootloader
	sleep 1

bootloader-alive: reset-device
	$(AVRDUDE_SERIAL)

flash-main-programmer: main.hex programmer-alive
	$(AVRDUDE_USB) -U flash:w:main.hex

flash-main-serial: main.hex bootloader-alive
	# avrdude does not support flash-erase for avr911
	# there seems to be a problem with verification, as some bytes appear to not arrive
	$(AVRDUDE_SERIAL) -e -D -V -U flash:w:main.hex

flash-fuses:
	$(AVRDUDE_USB) -U fuse0:w:$(FUSE0):m -U fuse1:w:$(FUSE1):m -U fuse2:w:$(FUSE2):m 
		-U fuse4:w:$(FUSE4):m -U fuse5:w:$(FUSE5):m 

flash-bootloader: bootloader.compiled
	$(AVRDUDE_USB) -U flash:w:Xmega_Bootloader/Xmega_Bootloader.hex -U eeprom:w:Xmega_Bootloader/Xmega_Bootloader.eep

flash-all: programmer-alive flash-fuses flash-bootloader flash-main-programmer

# debug: 
# 	$(CC) -ggdb -mmcu=atxmega128a1u -o main.elf main.o

################################################################################
## TEST                                                                       ##
################################################################################
main-test: $(SRC_FILES_TEST)
	$(CC_TEST) $(CCFLAGS_TEST) main.c -o $@

################################################################################
## ARM                                                                        ##
################################################################################
TARGET = main
OPT = -Og
DEBUG = 1
BUILD_DIR = build_arm
C_SOURCES_ARM = main.c
ASM_SOURCES_ARM = hal/arm/startup_stm32f446xx.s
PREFIX = arm-none-eabi-
CC_ARM = $(PREFIX)gcc
AS_ARM = $(PREFIX)gcc -x assembler-with-cpp
CP_ARM = $(PREFIX)objcopy
SZ_ARM = $(PREFIX)size
DBG_ARM = $(PREFIX)gdb
HEX = -O ihex
BIN = -O binary -S
CPU_ARM = -mcpu=cortex-m4
FPU_ARM = -mfpu=fpv4-sp-d16
FLOAT-ABI_ARM = -mfloat-abi=hard
MCU_ARM = $(CPU_ARM) -mthumb $(FPU_ARM) $(FLOAT-ABI_ARM)
AS_DEFS =
C_DEFS = -DXTRACTOR_ARCH_ARM
AS_INCLUDES =
C_INCLUDES = 
ASFLAGS_ARM = $(MCU_ARM) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -fdata-sections -ffunction-sections
CFLAGS_ARM = $(MCU_ARM) $(C_DEFS) $(C_INCLUDES) $(OPT) -fdata-sections -ffunction-sections $(CCFLAGS_GENERAL)
ifeq ($(DEBUG), 1)
CFLAGS_ARM += -g -gdwarf-2
endif
# Generate dependency information
CFLAGS_ARM += -MMD -MP -MF"$(@:%.o=%.d)"
CFLAGS_ARM += -DF_CPU=168000000

LDSCRIPT_ARM = hal/arm/STM32F446ZETx_FLASH.ld
LIBS_ARM = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS_ARM = $(MCU_ARM) -specs=nano.specs -T$(LDSCRIPT_ARM) $(LIBDIR) $(LIBS_ARM) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

arm: clean consume_flags $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin flash_arm FORCE

OBJECTS_ARM = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES_ARM:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES_ARM)))
# list of ASM program objects
OBJECTS_ARM += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES_ARM:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES_ARM)))

$(BUILD_DIR)/%.o: %.c Makefile FORCE | $(BUILD_DIR)
	$(CC_ARM) -c $(CFLAGS_ARM) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@
$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
		$(AS_ARM) -c $(CFLAGS_ARM) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS_ARM) Makefile
	$(CC_ARM) $(OBJECTS_ARM) $(LDFLAGS_ARM) -o $@
	$(SZ_ARM) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
		$(CP_ARM) $(HEX) $< $@
        
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
		$(CP_ARM) $(BIN) $< $@    
        
$(BUILD_DIR):
		mkdir $@ 

clean:
		-rm -fR $(BUILD_DIR)
		rm *.hex *.o *.bin  *.elf main-test bootloader.compiled || true
		cd Xmega_Bootloader/ && make clean
  
flash_arm:
		st-flash write $(BUILD_DIR)/main.bin 0x8000000
		st-flash reset
debug_arm:
		st-util -p 1234 &
		$(DBG_ARM) -tui --eval-command="target extended-remote :1234" \
		--eval-command="layout split" \
		$(BUILD_DIR)/$(TARGET).elf
		pkill "st-util"

.PHONY: FORCE
FORCE:
#dependencies		
-include $(wildcard $(BUILD_DIR)/*.d)

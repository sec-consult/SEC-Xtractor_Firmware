SELF_TEST=0

CC=avr-gcc
CCFLAGS=-mmcu=atxmega128a1 -Os -std=gnu99 -DXTRACTOR_ARCH_AVR -DF_CPU=32000000 -DSELF_TEST=$(SELF_TEST)

CC_TEST=gcc
CCFLAGS_TEST=-Os -std=gnu99 -DXTRACTOR_ARCH_TEST

SRC_FILES=*.c *.h modules/*.c
SRC_FILES_AVR=$(SRC_FILES) hal-avr/*.c hal-avr/*.h
SRC_FILES_TEST=$(SRC_FILES) hal-test/*.c hal-test/*.h

OBJECTS=*.o

LDFLAGS=-mmcu=atxmega128a1 -g

PLATFORM=x128a1

SERIAL_PORT=/dev/ttyUSB3
BAUD=4000000

AVRDUDE_USB=avrdude -p $(PLATFORM) -P usb -c atmelice_pdi 
AVRDUDE_SERIAL=avrdude -p $(PLATFORM) -P $(SERIAL_PORT) -b 115200 -c avr911

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
CCFLAGS+= -DESCAPE_CODE_SUPPORTED
endif

################################################################################
## GENERAL/MISC                                                               ##
################################################################################
all: main.hex

clean:
	rm *.hex *.o *.bin  *.elf main-test bootloader.compiled || true
	cd Xmega_Bootloader/ && make clean

test: main-test
	./main-test


################################################################################
## AVR DEVICE                                                                 ##
################################################################################

main.hex: main.bin
	avr-objcopy -j .text -j .data -O ihex $^ $@

main.bin: consume_flags $(SRC_FILES_AVR)
	$(CC) $(CCFLAGS) main.c -o $@

main.o: consume_flags $(SRC_FILES_AVR)
	$(CC) $(CCFLAGS) -c main.c -o $@

hal-avr: consume_flags $(SRC_FILES_AVR)
	$(CC) $(CCFLAGS) -c $(SRC_FILES_AVR) -o $^

main.elf: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

bootloader.bin: bootloader.c
	$(CC) $(CCFLAGS) bootloader.c -o $@

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

debug: 
	$(CC) -ggdb -mmcu=atxmega128a1u -o main.elf main.o

################################################################################
## TEST                                                                       ##
################################################################################
main-test: $(SRC_FILES_TEST)
	$(CC_TEST) $(CCFLAGS_TEST) main.c -o $@


# SEC Xtractor (Firmware)
A tool for directly dumping memory chips and identifying on-chip debugging/programming interfaces. Can act as UART-to-USB converter and JTAG adapter. 

# Documentation
A short documentation for the usage of the SEC Xtractor can be found in the [hardware repository][9] under the folder *documentation*.

# Dependencies
*For compiling and programming the SEC Xtractor*
avr-binutils
avr-gcc
avr-libc
avrdude
*For configuring the FT2232HQ*
ftdi_eeprom

# Programming
To update the main program of the SEC Xtractor, no additional programmer is needed. This is only required for changing the bootloader.
## Serial
Connect the device with a USB cable to your PC. Navigate into the root of the directory and compile the program with "make". Afterwards, type "make flash-main-serial" to burn the main program to the flash memory.
## PDI
To program the device with PDI, an additional programmer like the AVRISPMKII or the ATMELICE is needed. The command "make flash-main-programmer" can be used to flash the bootloader (this is only possible with a programmer).
## FT2232HQ EEPROM
The EEPROM of the FTDI chip can be changed with "ftdi_eeprom --flash-eeprom <config-file>". This can be done to change the manufacturer, the product name and the serial string.
# Authors
Initial version:
* Thomas Weber of [SEC Consult][1]

Rolling relase:
* Thomas Weber of [SEC Consult][1]
* [Wolfgang Ettlinger][2] of [SEC Consult][1]
* [Steffen Robertz][4] of [SEC Consult][1]

# Other Used Projects
The firmware of SEC Xtractor was written from scratch with two exceptions:
* JTAG brute forcing code was used from [JTAGenum][5] [Nathan Andrew Fain][6]
* The [Xmega_bootloader][7] from [Anthony Andriano][8] was directly used

## License
The SEC Xtractor source code is distributed under a [Mozilla Public License 2.0][3] copyleft license. This means that it requires modifications to be shared.

[1]: https://www.sec-consult.com
[2]: https://twitter.com/ettisan
[3]: https://www.mozilla.org/en-US/MPL/2.0/
[4]: https://www.linkedin.com/in/shr70/
[5]: https://github.com/cyphunk/JTAGenum
[6]: https://twitter.com/cyphunk
[7]: https://github.com/bandtank/Xmega_Bootloader
[8]: https://github.com/bandtank
[9]: https://github.com/sec-consult/SEC-Xtractor_Hardware

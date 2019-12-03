/**
 * @file spi.c
  * 
 * Copyright (C): SEC Consult Unternehmensberatung GmbH, 2019         \n
 * Web:           https://sec-consult.com/                            \n
 *                                                                    \n
 * This Source Code Form is subject to the terms of the Mozilla Public\n
 * License, v. 2.0. If a copy of the MPL was not distributed with this\n
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.          \n
 * 
 * @authors Thomas Weber, Wolfgang Ettlinger
 */
/******************** SPI GD5F1GQ4U *********************/
/*DOES NOT WORK YET*/
volatile uint8_t spiTransferComplete;

ISR(SPIC_INT_vect)
{
	spiTransferComplete = 1;
}
/*DOES NOT WORK YET*/
void spiConfigurePins()
{
	uint8_t rx_byte;
	PORTC.DIR = 0xB0;	//output: mosi, sck, ss
	PORTC.OUTSET = 0x10; //spi master mode 0
	SPIC.CTRL = 0x53;	// spi master, spi mode 0 and prescaler to [11]
	spiTransferComplete = 1;
	SPIC.INTCTRL = 0x03; // assign high priority to SPID interrupts

	while (SPIC.STATUS & 0x80)
	{ // flush spi receive buffer
		rx_byte = SPIC.DATA;
	}

	uartWriteString("\n\rconfigure\n\r");
	return;
}

/*DOES NOT WORK YET*/
uint8_t spiRead()
{
	//uartWriteString("Read\n\r");
	uint8_t byte = SPIC.DATA;
	return byte;
}

/*DOES NOT WORK YET*/
void spiWrite(uint8_t byte)
{
	//	uartWriteString("Write\n\r");
	spiTransferComplete = 0;
	SPIC.DATA = byte;
	while (!spiTransferComplete)
		; // wait for transmit complete
	spiTransferComplete = 0;
	return;
}

/*DOES NOT WORK YET*/
void spiGDreadID()
{
	PORTC.OUTCLR = 0x10; // assert SS pin (active low)
	NOP;
	spiWrite(0x9F);
	spiWrite(0x00);
	uartprintf("%02X", spiRead());
	uartprintf("%02X", spiRead());
	uartWriteString(NL);
	PORTC.OUTSET = 0x10; // de-assert SS pin
}

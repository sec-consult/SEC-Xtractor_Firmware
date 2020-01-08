/**
 * @file nand.c
 * 
 * Copyright (C): SEC Consult Unternehmensberatung GmbH, 2019         \n
 * Web:           https://sec-consult.com/                            \n
 *                                                                    \n
 * This Source Code Form is subject to the terms of the Mozilla Public\n
 * License, v. 2.0. If a copy of the MPL was not distributed with this\n
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.          \n
 * 
 * @authors Thomas Weber, Wolfgang Ettlinger, Steffen Robertz
 */
#include "definitions.h"
#include "../secxtractor.h"

#define NAND_ALE_HIGH()			{ NAND_CTRL_PORT.OUTSET = NAND_ALE; }
#define NAND_ALE_LOW()			{ NAND_CTRL_PORT.OUTCLR = NAND_ALE; }
#define NAND_CLE_HIGH() 		{ NAND_CTRL_PORT.OUTSET = NAND_CLE; }
#define NAND_CLE_LOW()			{ NAND_CTRL_PORT.OUTCLR = NAND_CLE; }
#define NAND_WP_HIGH()			{ NAND_CTRL_PORT.OUTSET = NAND_WP; }
#define NAND_WP_LOW()			{ NAND_CTRL_PORT.OUTCLR = NAND_WP; }
#define NAND_WE_HIGH()			{ NAND_CTRL_PORT.OUTSET = NAND_WE; }
#define NAND_WE_LOW()			{ NAND_CTRL_PORT.OUTCLR = NAND_WE; }
#define NAND_RE_HIGH()			{ NAND_CTRL_PORT.OUTSET = NAND_RE; }
#define NAND_RE_LOW()			{ NAND_CTRL_PORT.OUTCLR = NAND_RE; }
#define NAND_CE_HIGH()			{ NAND_CTRL_PORT.OUTSET = NAND_CE; }
#define NAND_CE_LOW()			{ NAND_CTRL_PORT.OUTCLR = NAND_CE; }
#define NAND_RB_PULLUP_HIGH() 	{ NAND_CTRL_PORT.OUTSET = NAND_RB; }

#define NAND_RB_READ() (((NAND_CTRL_PORT.IN & NAND_RB) == NAND_RB) ? 1 : 0)

/**
*@brief	writes a command byte to the nand according to ONFI standard
*/
static inline void onfiCommand(uint8_t command)
{
	NAND_IO_PORT0.OUT = 0x00;
	NAND_IO_PORT0.DIR = 0xFF;
	NAND_IO_PORT1.OUT = 0x00;
	NAND_IO_PORT1.DIR = 0xFF;
	NAND_CE_LOW();
	NAND_CLE_LOW();
	NAND_ALE_LOW();
	NAND_WE_HIGH();
	NAND_CLE_HIGH();
	NAND_WE_LOW();
	NAND_IO_PORT0.OUT = command;
	NAND_WE_HIGH();
	NAND_CLE_LOW();
}

/**
*@brief	inits pins for an address command
*/
static inline void onfiAddressStart(void)
{
	NAND_CLE_LOW();
	NAND_CE_LOW();
	NAND_WE_HIGH();
	NAND_ALE_HIGH();
	NAND_IO_PORT1.OUT = 0x00;
}

/**
*@brief	writes an address byte to the nand according to ONFI standard
*/
static inline void onfiAddress(uint8_t address)
{
	NAND_WE_LOW();
	NAND_IO_PORT0.OUT = address;
	NAND_WE_HIGH();
}

/**
*@brief	finishes the address transfer cycle
*/
static inline void onfiAddressEnd(void)
{
	NAND_ALE_LOW();
}

/**
*@brief	prepares pins to receive data from NAND
*/
static inline void onfiDataOutStart()
{
	NAND_IO_PORT0.OUT = 0x00;
	NAND_IO_PORT0.DIR = 0x00;
	NAND_IO_PORT1.OUT = 0x00;
	NAND_IO_PORT1.DIR = 0x00;
}

/**
*@brief	receives a byte from the NAND
*/
static inline void onfiDataOut(uint8_t *out_low, uint8_t *out_high)
{
	NAND_RE_LOW();
	NOP;
	*out_high = NAND_IO_PORT1.IN;
	*out_low = NAND_IO_PORT0.IN;
	NAND_RE_HIGH();
}

/**
*@brief	prepares pins for a write to the NAND
*/
static inline void onfiDataInStart(void)
{
	NAND_CLE_LOW();
	NAND_CE_LOW();
	NAND_ALE_LOW();
}

/**
*@brief	writes a data byte to the NAND according to ONFI standard
*/
static inline void onfiDataIn(uint8_t data_low, uint8_t data_high)
{
	NAND_WE_LOW();
	NAND_IO_PORT0.OUT = data_low;
	NAND_IO_PORT1.OUT = data_high;
	NAND_WE_HIGH();
}

/**
*@brief	resets the NAND
*/
static inline void nandReset()
{
	onfiCommand(0xff);
	_delay_us(5);
	while(!NAND_RB_READ());
}

/**
*@brief	Initializes the NAND and the required pins
*/
static inline void nandInit()
{
	/* set all pins to outputs to issue the first command */
	NAND_IO_PORT0.OUT = 0x00;
	NAND_IO_PORT1.OUT = 0x00;
	NAND_IO_PORT0.DIRSET = 0xFF;
	NAND_IO_PORT1.DIRSET = 0xFF;

	NAND_CE_HIGH();
	NAND_WE_HIGH();
	NAND_RE_HIGH();
	NAND_CLE_LOW();
	NAND_ALE_LOW();
	NAND_WP_LOW();
	NAND_RB_PULLUP_HIGH();

	NAND_CTRL_PORT.DIRSET = (NAND_ALE) | (NAND_CLE) | (NAND_CE) | (NAND_RE) | (NAND_WP) | (NAND_WE);
	NAND_CTRL_PORT.DIRCLR = NAND_RB;
}

/**
*@brief			reads the ONFI signature
*@param	result	array where the ONFI chars schould be saved
*/
static inline void nandReadONFI(uint8_t result[])
{
	uint8_t waste;
	onfiCommand(0x90);
	onfiAddressStart();
	onfiAddress(0x00);
	onfiAddressEnd();
	onfiDataOutStart();
	onfiDataOut(&result[0], &waste);
	onfiDataOut(&result[1], &waste);
	onfiDataOut(&result[2], &waste);
	onfiDataOut(&result[3], &waste);
	NAND_CE_HIGH();
}

/**
*@brief			reads the Chip ID
*@param	result	array where the Chip ID schould be saved
*/
static inline void nandReadChipID(uint8_t *result)
{
	uint8_t waste;
	onfiCommand(0x90);
	onfiAddressStart();
	onfiAddress(0x00);
	onfiAddressEnd();
	onfiDataOutStart();
	onfiDataOut(&result[0], &waste);
	onfiDataOut(&result[1], &waste);
	onfiDataOut(&result[2], &waste);
	onfiDataOut(&result[3], &waste);
	onfiDataOut(&result[4], &waste);
	NAND_CE_HIGH();

}

/**
*@brief					Erases the whole NAND
*@param	params			Pointer to the ONFI parameter page
*@param enable_progress	Will print dots as progress indication if set to 1
*/
static inline void onfiEraseAll(onfi_param_page_t *params, uint8_t enable_progress)
{
	NAND_IO_PORT0.OUT = 0x00;
	NAND_IO_PORT1.OUT = 0x00;
	NAND_IO_PORT0.DIR = 0xFF;
	NAND_IO_PORT1.DIR = 0xFF;
	uint8_t page_bits = 255; //Loop counts one bit too much
	for(uint32_t tmp_pages_per_block = params->pages_per_block;\
	tmp_pages_per_block != 0; tmp_pages_per_block>>=1){
		page_bits++;
	}
	uint8_t row_cycles = params->address_cycles &0x0f;
	uint8_t col_cycles = params->address_cycles >> 4;
	uint32_t block=0;
	NAND_WP_HIGH();
	while(block<params->block_per_logic_unit){
		onfiCommand(0x60);
		onfiAddressStart();
		uint64_t row_address = (((uint64_t)block)<<page_bits);
		for(uint8_t i=0; i<row_cycles; i++) {
			onfiAddress((uint8_t)(row_address>>(i*8)));
		}
		onfiAddressEnd();
		onfiCommand(0xd0);
		NOP3;
		while(!NAND_RB_READ());
		if(enable_progress & block%20==0){
			uartWriteChar('.');
		}
		++block;
	}
	NAND_WP_LOW();
}

/**
*@brief				Writes a pattern throughout the whole NAND
*@param	params		Pointer to the ONFI parameter page
*@param pattern		pattern to write
*@param write_ecc	If set to 1, will write through ECC block, otherwise ECC block will be skipped
*/
static inline void onfiWriteData(onfi_param_page_t *params, uint16_t pattern, uint8_t write_ecc)
{
	uint8_t supports_x16 = params->feature_support & 0x0001;
	uint8_t row_cycles = params->address_cycles &0x0f;
	uint8_t col_cycles = params->address_cycles >> 4;
	uint8_t page_bits = 255; //Loop counts one bit too much
	for(uint32_t tmp_pages_per_block = params->pages_per_block;\
	tmp_pages_per_block != 0; tmp_pages_per_block>>=1){
		page_bits++;
	}

	uint32_t block = 0x00;
	uint32_t page = 0x00;
	uint32_t pagesize = params->bytes_per_page +(write_ecc*params->spare_bytes_per_page);
	pagesize = pagesize / (supports_x16+1);

	onfiEraseAll(params, 0);

	while (block < params->block_per_logic_unit){ 
		NAND_WP_HIGH();
		onfiCommand(0x80);
		onfiAddressStart();
		for(uint8_t i=0; i<col_cycles; ++i){
			onfiAddress(0x00);
		}
		uint64_t row_address = page;
		row_address |= (((uint64_t)block)<<page_bits);
		for(uint8_t i=0; i<row_cycles; i++) {
			onfiAddress((uint8_t)(row_address>>(i*8)));
		}
		onfiAddressEnd();
		NOP2;
		onfiDataInStart();
		for (int i = 0; i < pagesize; i++)
		{
			if(supports_x16){
				onfiDataIn((uint8_t) pattern,(uint8_t) (pattern >> 8));
			}else{
				if(i%2){
					onfiDataIn((uint8_t) pattern, 0);
				}else{
					onfiDataIn((uint8_t) pattern>>8, 0);
				}
			}
		}
		onfiCommand(0x10);
		_delay_us(10);
		while(!NAND_RB_READ());
		onfiCommand(0x70);
		uint8_t temp1, temp2;
		onfiDataOut(&temp1, &temp2);
		NAND_CE_HIGH();
		NAND_WP_LOW();
		page++;
		if (page == 64)
		{
			page = 0;
			block++;
		}
		if(page==0 && block%10==0){
			uartWriteChar('.');
		}
	}
	uartprintf("done"NL);
}

/**
*@brief			Reads the ONFI parameters
*@param	params	Pointer to an empty full parameter page
*/
static inline void onfiReadParameters(onfi_param_page_full_t *params_full)
{
	onfiCommand(0xec);
	onfiAddressStart();
	onfiAddress(0x00);
	onfiAddressEnd();
	onfiDataOutStart();
	uint8_t *data = (uint8_t*) params_full;
	uint8_t waste;
	_delay_us(10);
	while(!NAND_RB_READ());
    for(uint16_t i = 0; i<256*3; i++){
	    onfiDataOut(&data[i], &waste);
    }
	NAND_CE_HIGH();
}

/**
*@brief				Writes a pattern throughout the whole NAND
*@param	params		Pointer to the ONFI parameter page
*@param read_ecc	If set to 1, will read with ECC block, otherwise ECC block will be skipped
*/
static inline void onfiReadData(onfi_param_page_t *params, uint8_t read_ecc)
{
	uint8_t supports_x16 = params->feature_support & 0x0001;
	uint8_t row_cycles = params->address_cycles &0x0f;
	uint8_t col_cycles = params->address_cycles >> 4;
	uint8_t page_bits = 255; //Loop counts one bit too much
	for(uint32_t tmp_pages_per_block = params->pages_per_block;\
	tmp_pages_per_block != 0; tmp_pages_per_block>>=1){
		page_bits++;
	}

	uint32_t block = 0x00;
	uint32_t page = 0x00;
	dumpStart();
	while (block < params->block_per_logic_unit && !isOperationCanceled())
	{ 
		NAND_IO_PORT0.OUT = 0x00;
		NAND_IO_PORT1.OUT = 0x00;
		NAND_IO_PORT0.DIR = 0xFF;
		NAND_IO_PORT1.DIR = 0xFF;
		onfiCommand(0x00);
		onfiAddressStart();
		for(uint8_t i=0; i<col_cycles; ++i){
			onfiAddress(0x00);
		}
		uint64_t row_address = page;
		row_address |= (((uint64_t)block)<<page_bits);
		for(uint8_t i=0; i<row_cycles; i++) {
			onfiAddress((uint8_t)(row_address>>(i*8)));
		}
		onfiAddressEnd();
		/*jump to next page. increase the block if the max page count is reached*/
		page++;
		if (page == params->pages_per_block)
		{
			page = 0;
			block++;
		}
		onfiCommand(0x30);
		NOP2;
		while(!NAND_RB_READ());
		/*end of sending command, start reading the whole page*/
		onfiDataOutStart();
		uint32_t total_bytes = params->bytes_per_page +(read_ecc*params->spare_bytes_per_page);
		total_bytes = total_bytes / (supports_x16+1);
		
		for (int i = 0; i < total_bytes && !isOperationCanceled(); ++i)
		{
			uint8_t temp_high = 0;
			uint8_t temp_low = 0;
			onfiDataOut(&temp_low, &temp_high);
			if(supports_x16){
				dumpByte(temp_high);
			}
			dumpByte(temp_low);
		}
		NAND_CE_HIGH();
	}
	dumpEnd();
	uartWriteString(NL "reading done" NL);
}


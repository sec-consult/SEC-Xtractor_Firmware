// /**
//  * @file nand.c
//  * 
//  * Copyright (C): SEC Consult Unternehmensberatung GmbH, 2019         \n
//  * Web:           https://sec-consult.com/                            \n
//  *                                                                    \n
//  * This Source Code Form is subject to the terms of the Mozilla Public\n
//  * License, v. 2.0. If a copy of the MPL was not distributed with this\n
//  * file, You can obtain one at https://mozilla.org/MPL/2.0/.          \n
//  * 
//  * @authors Thomas Weber, Wolfgang Ettlinger, Steffen Robertz
//  */

// #include "../../hal.h"

// /**
// *@brief	writes a command byte to the nand according to ONFI standard
// */
// static inline void onfiCommand(uint8_t command)
// {
// 	setNandIOValue
// 	NAND_IO_PORT0.OUT = 0x00;
// 	NAND_IO_PORT0.DIR = 0xFF;
// 	NAND_IO_PORT1.OUT = 0x00;
// 	NAND_IO_PORT1.DIR = 0xFF;
// 	NAND_CE_LOW();
// 	NAND_CLE_LOW();
// 	NAND_ALE_LOW();
// 	NAND_WE_HIGH();
// 	NAND_CLE_HIGH();
// 	NAND_WE_LOW();
// 	NAND_IO_PORT0.OUT = command;
// 	NAND_WE_HIGH();
// 	NAND_CLE_LOW();
// }

// /**
// *@brief	inits pins for an address command
// */
// static inline void onfiAddressStart(void)
// {
// 	NAND_CLE_LOW();
// 	NAND_CE_LOW();
// 	NAND_WE_HIGH();
// 	NAND_ALE_HIGH();
// 	NAND_IO_PORT1.OUT = 0x00;
// }

// /**
// *@brief	writes an address byte to the nand according to ONFI standard
// */
// static inline void onfiAddress(uint8_t address)
// {
// 	NAND_WE_LOW();
// 	NAND_IO_PORT0.OUT = address;
// 	NAND_WE_HIGH();
// }

// /**
// *@brief	finishes the address transfer cycle
// */
// static inline void onfiAddressEnd(void)
// {
// 	NAND_ALE_LOW();
// }

// /**
// *@brief	prepares pins to receive data from NAND
// */
// static inline void onfiDataOutStart()
// {
// 	NAND_IO_PORT0.OUT = 0x00;
// 	NAND_IO_PORT0.DIR = 0x00;
// 	NAND_IO_PORT1.OUT = 0x00;
// 	NAND_IO_PORT1.DIR = 0x00;
// }

// /**
// *@brief	receives a byte from the NAND
// */
// static inline void onfiDataOut(uint8_t *out_low, uint8_t *out_high)
// {
// 	NAND_RE_LOW();
// 	NOP10;
// 	NAND_DELAY();
// 	*out_high = NAND_IO_PORT1.IN;
// 	*out_low = NAND_IO_PORT0.IN;
// 	NAND_RE_HIGH();
// }

// /**
// *@brief	prepares pins for a write to the NAND
// */
// static inline void onfiDataInStart(void)
// {
// 	NAND_CLE_LOW();
// 	NAND_CE_LOW();
// 	NAND_ALE_LOW();
// }

// /**
// *@brief	writes a data byte to the NAND according to ONFI standard
// */
// static inline void onfiDataIn(uint8_t data_low, uint8_t data_high)
// {
// 	NAND_WE_LOW();
// 	NAND_IO_PORT0.OUT = data_low;
// 	NAND_IO_PORT1.OUT = data_high;
// 	NAND_WE_HIGH();
// 	NOP;
// }

// /**
// *@brief	resets the NAND
// */
// static inline void nandReset()
// {
// 	onfiCommand(ONFI_CMD_RESET);
// 	_delay_us(5);
// 	while(!NAND_RB_READ());
// }

// /**
// *@brief	Initializes the NAND and the required pins
// */
// static inline void nandInit()
// {
// 	/* set all pins to outputs to issue the first command */
// 	NAND_IO_PORT0.OUT = 0x00;
// 	NAND_IO_PORT1.OUT = 0x00;
// 	NAND_IO_PORT0.DIRSET = 0xFF;
// 	NAND_IO_PORT1.DIRSET = 0xFF;

// 	NAND_CE_HIGH();
// 	NAND_WE_HIGH();
// 	NAND_RE_HIGH();
// 	NAND_CLE_LOW();
// 	NAND_ALE_LOW();
// 	NAND_WP_LOW();
// 	NAND_RB_PULLUP_HIGH();

// 	NAND_CTRL_PORT.DIRSET = (NAND_ALE) | (NAND_CLE) | (NAND_CE) | (NAND_RE) | (NAND_WP) | (NAND_WE);
// 	NAND_CTRL_PORT.DIRCLR = NAND_RB;
// }

// /**
// *@brief			reads the ONFI signature
// *@param	result	array where the ONFI chars schould be saved
// */
// static inline void nandReadONFI(uint8_t result[])
// {
// 	uint8_t waste;
// 	onfiCommand(ONFI_CMD_READ_ID);
// 	onfiAddressStart();
// 	onfiAddress(0x20);
// 	onfiAddressEnd();
// 	onfiDataOutStart();
// 	onfiDataOut(&result[0], &waste);
// 	onfiDataOut(&result[1], &waste);
// 	onfiDataOut(&result[2], &waste);
// 	onfiDataOut(&result[3], &waste);
// 	NAND_CE_HIGH();
// }

// /**
// *@brief			reads the Chip ID
// *@param	result	array where the Chip ID schould be saved
// */
// static inline void nandReadChipID(uint8_t *result)
// {
// 	uint8_t waste;
// 	onfiCommand(ONFI_CMD_READ_ID);
// 	onfiAddressStart();
// 	onfiAddress(0x00);
// 	onfiAddressEnd();
// 	onfiDataOutStart();
// 	onfiDataOut(&result[0], &waste);
// 	onfiDataOut(&result[1], &waste);
// 	onfiDataOut(&result[2], &waste);
// 	onfiDataOut(&result[3], &waste);
// 	onfiDataOut(&result[4], &waste);
// 	NAND_CE_HIGH();

// }

// /**
// *@brief					Erases the whole NAND
// *@param	params			Pointer to the ONFI parameter page
// *@param enable_progress	Will print dots as progress indication if set to 1
// */
// static inline void onfiEraseAll(onfi_param_page_t *params, uint8_t enable_progress)
// {
// 	NAND_IO_PORT0.OUT = 0x00;
// 	NAND_IO_PORT1.OUT = 0x00;
// 	NAND_IO_PORT0.DIR = 0xFF;
// 	NAND_IO_PORT1.DIR = 0xFF;
// 	uint8_t page_bits = 255; //Loop counts one bit too much
// 	for(uint32_t tmp_pages_per_block = params->pages_per_block;\
// 	tmp_pages_per_block != 0; tmp_pages_per_block>>=1){
// 		page_bits++;
// 	}
// 	uint8_t row_cycles = params->address_cycles &0x0f;
// 	uint8_t col_cycles = params->address_cycles >> 4;
// 	uint32_t block=0;
// 	NAND_WP_HIGH();
// 	while(block<params->block_per_logic_unit){
// 		onfiCommand(ONFI_CMD_BLOCK_ERASE_CYCLE_1);
// 		onfiAddressStart();
// 		uint64_t row_address = (((uint64_t)block)<<page_bits);
// 		for(uint8_t i=0; i<row_cycles; i++) {
// 			onfiAddress((uint8_t)(row_address>>(i*8)));
// 		}
// 		onfiAddressEnd();
// 		onfiCommand(ONFI_CMD_BLOCK_ERASE_CYCLE_2);
// 		NOP3;
// 		while(!NAND_RB_READ());
// 		if(enable_progress & block%20==0){
// 			uartWriteChar('.');
// 		}
// 		++block;
// 	}
// 	NAND_WP_LOW();
// }

// /**
// *@brief				Writes a pattern throughout the whole NAND
// *@param	params		Pointer to the ONFI parameter page
// *@param pattern		pattern to write
// *@param write_ecc	If set to 1, will write through ECC block, otherwise ECC block will be skipped
// */
// static inline void onfiWriteData(onfi_param_page_t *params, uint16_t pattern, uint8_t write_ecc)
// {
// 	uint8_t supports_x16 = params->feature_support & ONFI_FEATURE_16_BIT_BUS;
// 	uint8_t row_cycles = params->address_cycles &0x0f;
// 	uint8_t col_cycles = params->address_cycles >> 4;
// 	uint8_t page_bits = 255; //Loop counts one bit too much
// 	for(uint32_t tmp_pages_per_block = params->pages_per_block;\
// 	tmp_pages_per_block != 0; tmp_pages_per_block>>=1){
// 		page_bits++;
// 	}

// 	uint32_t block = 0x00;
// 	uint32_t page = 0x00;
// 	uint32_t pagesize = params->bytes_per_page +(write_ecc*params->spare_bytes_per_page);
// 	pagesize = pagesize / (supports_x16+1);

// 	onfiEraseAll(params, 0);

// 	while (block < params->block_per_logic_unit){ 
// 		NAND_WP_HIGH();
// 		onfiCommand(ONFI_CMD_PAGE_CACHE_PROGRAM_CYCLE_1);
// 		onfiAddressStart();
// 		for(uint8_t i=0; i<col_cycles; ++i){
// 			onfiAddress(0x00);
// 		}
// 		uint64_t row_address = page;
// 		row_address |= (((uint64_t)block)<<page_bits);
// 		for(uint8_t i=0; i<row_cycles; i++) {
// 			onfiAddress((uint8_t)(row_address>>(i*8)));
// 		}
// 		onfiAddressEnd();
// 		NOP2;
// 		onfiDataInStart();
// 		for (int i = 0; i < pagesize; i++)
// 		{
// 			if(supports_x16){
// 				onfiDataIn((uint8_t) pattern,(uint8_t) (pattern >> 8));
// 			}else{
// 				if(i%2){
// 					onfiDataIn((uint8_t) pattern, 0);
// 				}else{
// 					onfiDataIn((uint8_t) pattern>>8, 0);
// 				}
// 			}
// 		}
// 		onfiCommand(ONFI_CMD_PAGE_CACHE_PROGRAM_CYCLE_2);
// 		_delay_us(10);
// 		while(!NAND_RB_READ());
// 		onfiCommand(ONFI_CMD_READ_STATUS);
// 		uint8_t temp1, temp2;
// 		onfiDataOut(&temp1, &temp2);
// 		NAND_CE_HIGH();
// 		NAND_WP_LOW();
// 		page++;
// 		if (page == 64)
// 		{
// 			page = 0;
// 			block++;
// 		}
// 		if(page==0 && block%10==0){
// 			uartWriteChar('.');
// 		}
// 	}
// 	uartprintf("done"NL);
// }

// /**
// *@brief			Reads the ONFI parameters
// *@param	params	Pointer to an empty full parameter page
// */
// static inline int onfiReadAndCheckParameters(onfi_param_page_t *params)
// {
// 	uint8_t tmp;
// 	int result = 0;
// 	uint8_t *data = (uint8_t*) params;
// 	uint8_t waste;

// 	onfiCommand(ONFI_CMD_READ_PARAMETER_PAGE);
// 	onfiAddressStart();
// 	onfiAddress(0x00);
// 	onfiAddressEnd();
// 	onfiDataOutStart();
	
// 	_delay_us(10);
// 	while(!NAND_RB_READ());

// 	/* first: read params page */
//     for(uint16_t i = 0; i<256; i++){
// 	    onfiDataOut(&data[i], &waste);
//     }

// 	/* then: verify using other copies */
// 	for(uint16_t j = 0; j < 2; j++){
// 		for(uint16_t i = 0; i<256; i++){
// 			onfiDataOut(&tmp, &waste);
// 			if(tmp != data[i]){
// 				result |= 1;
// 			}
// 		}
// 	}

// 	NAND_CE_HIGH();

// 	if(!memcmp(&params->signature, "ONFI", 4) == 0){
// 		result |= 2;
// 	}

// 	return result;
// }

// static inline uint32_t onfiDumpPage(uint32_t block, uint32_t page, uint8_t supports_x16, uint8_t row_cycles, 
// 	uint8_t col_cycles, uint8_t page_bits, uint32_t bytes_to_read, uint8_t silent)
// {
// 	uint32_t checksum = 0;

// 	NAND_IO_PORT0.OUT = 0x00;
// 	NAND_IO_PORT1.OUT = 0x00;
// 	NAND_IO_PORT0.DIR = 0xFF;
// 	NAND_IO_PORT1.DIR = 0xFF;
// 	onfiCommand(ONFI_CMD_READ_CYCLE_1);
// 	onfiAddressStart();
// 	/* column addressing */
// 	for(uint8_t i=0; i<col_cycles; ++i){
// 		onfiAddress(0x00);
// 	}
// 	/* row addressing */
// 	uint64_t row_address = page;
// 	row_address |= (((uint64_t)block)<<page_bits);
// 	for(uint8_t i=0; i<row_cycles; i++) {
// 		onfiAddress((uint8_t)(row_address>>(i*8)));
// 	}
// 	onfiAddressEnd();
// 	onfiCommand(ONFI_CMD_READ_CYCLE_2);
// 	NOP2;
// 	while(!NAND_RB_READ());
// 	/* end of sending command, start reading the whole page */
// 	onfiDataOutStart();
// 	bytes_to_read = bytes_to_read / (supports_x16 + 1);
// 	for (int i = 0; i < bytes_to_read; ++i)
// 	{
// 		uint8_t temp_high = 0;
// 		uint8_t temp_low = 0;
// 		onfiDataOut(&temp_low, &temp_high);
// 		if(!silent){
// 			if(supports_x16){
// 				dumpByte(temp_high);
// 			}
// 			dumpByte(temp_low);
// 		}
// 		checksum += (temp_high << 8) + temp_low;
// 	}
// 	NAND_CE_HIGH();
// 	NAND_DELAY();
// 	return checksum;
// }

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
#include "../../secxtractor.h"
#include "../../hal.h"


/**
*@brief	writes a command byte to the nand according to ONFI standard
*/
static inline void onfiCommand(uint8_t command)
{
	setNandIOValue(0x0000);
	setNandIODir(OUT);
	// setPinValue(NAND_CTRL_PORT.ce, LOW);
	// setPinValue(NAND_CTRL_PORT.cle, LOW);
	setPinValue(NAND_CTRL_PORT.we, HIGH);
	setPinValue(NAND_CTRL_PORT.ale, LOW);
	setPinValue(NAND_CTRL_PORT.ce, LOW);
	setPinValue(NAND_CTRL_PORT.cle, HIGH);
	setPinValue(NAND_CTRL_PORT.we, LOW);
	setNandIOValue(command);
	delay_ns(T_WP);
	setPinValue(NAND_CTRL_PORT.we, HIGH);
	delay_ns(T_CLH);
	setPinValue(NAND_CTRL_PORT.cle, LOW);
	setPinValue(NAND_CTRL_PORT.ale, HIGH);
	setPinValue(NAND_CTRL_PORT.ce, HIGH);
	while(!getNandRB());
}

/**
*@brief	inits pins for an address command
*/
static inline void onfiAddressStart(void)
{
	setPinValue(NAND_CTRL_PORT.cle, LOW);
	setPinValue(NAND_CTRL_PORT.ce, LOW);
	setPinValue(NAND_CTRL_PORT.we, HIGH);
	delay_ns(T_CS-T_ALS);
	setPinValue(NAND_CTRL_PORT.ale, HIGH);
	setNandIOValue(0x00FF);
}

/**
*@brief	writes an address byte to the nand according to ONFI standard
*/
static inline void onfiAddress(uint8_t address)
{
	setPinValue(NAND_CTRL_PORT.we, LOW);
	setNandIOValue(address);
	delay_ns(T_WP);
	setPinValue(NAND_CTRL_PORT.we, HIGH);
	delay_ns(T_WH);
}

/**
*@brief	finishes the address transfer cycle
*/
static inline void onfiAddressEnd(void)
{
	setPinValue(NAND_CTRL_PORT.ce, HIGH);
	setPinValue(NAND_CTRL_PORT.ale, LOW);
}

/**
*@brief	prepares pins to receive data from NAND
*/
static inline void onfiDataOutStart()
{
	setNandIOValue(0x0000);
	setNandIODir(IN);
	setPinValue(NAND_CTRL_PORT.ce, LOW);
}

/**
*@brief	receives a byte/word from the NAND
*/
static inline void onfiDataOut(uint8_t *out_low, uint8_t *out_high)
{
	setPinValue(NAND_CTRL_PORT.re, LOW);
	delay_ns(T_RP);
	//_delay_32mhz_cycles( 2 + nandDelayTime);
	getNandIOValue(out_low, out_high);
	setPinValue(NAND_CTRL_PORT.re, HIGH);
	delay_ns(T_REH);
}

/**
*@brief	receives a byte/word from the NAND
*/
static inline void onfiDataOutEnd()
{
	setPinValue(NAND_CTRL_PORT.ce, HIGH);
}

/**
*@brief	prepares pins for a write to the NAND
*/
static inline void onfiDataInStart(void)
{
	setPinValue(NAND_CTRL_PORT.cle, LOW);
	setPinValue(NAND_CTRL_PORT.ce, LOW);
	setPinValue(NAND_CTRL_PORT.ale, LOW);
}

/**
*@brief	writes a data byte to the NAND according to ONFI standard
*/
static inline void onfiDataIn(uint8_t data_low, uint8_t data_high)
{
	setPinValue(NAND_CTRL_PORT.we, LOW);
	setNandIOValue((((uint16_t)data_high)<<8)|data_low);
	setPinValue(NAND_CTRL_PORT.we, HIGH);
	//TODO: Get right value
	delay_ns(50);
}

/**
*@brief	resets the NAND
*/
static inline void nandReset()
{
	onfiCommand(ONFI_CMD_RESET);
	delay_ns(500);
	//_delay_32mhz_cycles(160);
	while(!getNandRB());
}

/**
*@brief	Initializes the NAND and the required pins
*/
static inline void nandInit()
{
	/* set all pins to outputs to issue the first command */
	setNandIOValue(0x0000);
	setNandIODir(OUT);

	setPinValue(NAND_CTRL_PORT.ce, HIGH);
	setPinValue(NAND_CTRL_PORT.we, HIGH);
	setPinValue(NAND_CTRL_PORT.re, HIGH);
	setPinValue(NAND_CTRL_PORT.cle, LOW);
	setPinValue(NAND_CTRL_PORT.ale, LOW);
	setPinValue(NAND_CTRL_PORT.wp, LOW);
	setPinDir(NAND_CTRL_PORT.ale, OUT);
	setPinDir(NAND_CTRL_PORT.cle, OUT);
	setPinDir(NAND_CTRL_PORT.ce, OUT);
	setPinDir(NAND_CTRL_PORT.re, OUT);
	setPinDir(NAND_CTRL_PORT.wp, OUT);
	setPinDir(NAND_CTRL_PORT.we, OUT);
	setInputMode(NAND_CTRL_PORT.r_b, GPIO_PU);
	setPinDir(NAND_CTRL_PORT.r_b, IN);
}

/**
*@brief			reads the ONFI signature
*@param	result	array where the ONFI chars schould be saved
*/
static inline void nandReadONFI(uint8_t result[])
{
	uint8_t waste;
	onfiCommand(ONFI_CMD_READ_ID);
	onfiAddressStart();
	onfiAddress(0x20);
	onfiAddressEnd();
	onfiDataOutStart();
	onfiDataOut(&result[0], &waste);
	onfiDataOut(&result[1], &waste);
	onfiDataOut(&result[2], &waste);
	onfiDataOut(&result[3], &waste);
	setPinValue(NAND_CTRL_PORT.ce, HIGH);
}

/**
*@brief			reads the Chip ID
*@param	result	array where the Chip ID schould be saved
*/
static inline void nandReadChipID(uint8_t *result)
{
	uint8_t waste;
	onfiCommand(ONFI_CMD_READ_ID);
	onfiAddressStart();
	onfiAddress(0x00);
	onfiAddressEnd();
	onfiDataOutStart();
	onfiDataOut(&result[0], &waste);
	onfiDataOut(&result[1], &waste);
	onfiDataOut(&result[2], &waste);
	onfiDataOut(&result[3], &waste);
	onfiDataOut(&result[4], &waste);
	setPinValue(NAND_CTRL_PORT.ce, HIGH);

}

/**
*@brief					Erases the whole NAND
*@param	params			Pointer to the ONFI parameter page
*@param enable_progress	Will print dots as progress indication if set to 1
*/
static inline void onfiEraseAll(onfi_param_page_t *params, uint8_t enable_progress)
{
	setNandIOValue(0x0000);
	setNandIODir(OUT);
	uint8_t page_bits = 255; //Loop counts one bit too much
	for(uint32_t tmp_pages_per_block = params->pages_per_block;\
	tmp_pages_per_block != 0; tmp_pages_per_block>>=1){
		page_bits++;
	}
	uint8_t row_cycles = params->address_cycles &0x0f;
	uint8_t col_cycles = params->address_cycles >> 4;
	uint32_t block=0;
	setPinValue(NAND_CTRL_PORT.wp, HIGH);
	while(block<params->block_per_logic_unit){
		onfiCommand(ONFI_CMD_BLOCK_ERASE_CYCLE_1);
		onfiAddressStart();
		uint64_t row_address = (((uint64_t)block)<<page_bits);
		for(uint8_t i=0; i<row_cycles; i++) {
			onfiAddress((uint8_t)(row_address>>(i*8)));
		}
		onfiAddressEnd();
		onfiCommand(ONFI_CMD_BLOCK_ERASE_CYCLE_2);
		//TODO:get right value
		//delay_ns(40);
		//_delay_32mhz_cycles(3);
		//while(!getNandRB());
		if(enable_progress & block%20==0){
			uartWriteChar('.');
		}
		++block;
	}
	setPinValue(NAND_CTRL_PORT.wp, LOW);
}

/**
*@brief				Writes a pattern throughout the whole NAND
*@param	params		Pointer to the ONFI parameter page
*@param pattern		pattern to write
*@param write_ecc	If set to 1, will write through ECC block, otherwise ECC block will be skipped
*/
static inline void onfiWriteData(onfi_param_page_t *params, uint16_t pattern, uint8_t write_ecc)
{
	uint8_t supports_x16 = params->feature_support & ONFI_FEATURE_16_BIT_BUS;
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
		setPinValue(NAND_CTRL_PORT.wp, HIGH);
		onfiCommand(ONFI_CMD_PAGE_CACHE_PROGRAM_CYCLE_1);
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
		//TODO: get right value
		delay_ns(50);
		//_delay_32mhz_cycles(2);
		onfiDataInStart();
		for (int i = 0; i < pagesize; i++)
		{
			if(supports_x16){
				//onfiDataIn((uint8_t) pattern,(uint8_t) (pattern >> 8));
				onfiDataIn(i*2,i*2+1);
			}else{
				if(i%2){
					onfiDataIn((uint8_t) block, 0);
				}else{
					onfiDataIn((uint8_t) page, 0);
				}
			}
		}
		onfiCommand(ONFI_CMD_PAGE_CACHE_PROGRAM_CYCLE_2);
		//TODO: get right value
		delay_ns(100);
		//_delay_32mhz_cycles(10);
		while(!getNandRB());
		onfiCommand(ONFI_CMD_READ_STATUS);
		uint8_t temp1, temp2;
		onfiDataOut(&temp1, &temp2);
		setPinValue(NAND_CTRL_PORT.ce, HIGH);
		setPinValue(NAND_CTRL_PORT.wp, LOW);
		page++;
		if (page == params->pages_per_block)
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
static inline int onfiReadAndCheckParameters(onfi_param_page_t *params)
{
	uint8_t tmp;
	int result = 0;
	uint8_t *data = (uint8_t*) params;
	uint8_t waste;

	onfiCommand(ONFI_CMD_READ_PARAMETER_PAGE);
	onfiAddressStart();
	onfiAddress(0x00);
	onfiAddressEnd();
	onfiDataOutStart();
	
	//_delay_32mhz_cycles(2);
	while(!getNandRB());

	/* first: read params page */
    for(uint16_t i = 0; i<256; i++){
	    onfiDataOut(&data[i], &waste);
    }

	/* then: verify using other copies */
	for(uint16_t j = 0; j < 2; j++){
		for(uint16_t i = 0; i<256; i++){
			onfiDataOut(&tmp, &waste);
			if(tmp != data[i]){
				result |= 1;
			}
		}
	}
	onfiDataOutEnd();
	if(!memcmp(&params->signature, "ONFI", 4) == 0){
		result |= 2;
	}

	return result;
}
static inline uint32_t onfiDumpPage(uint32_t block, uint32_t page, uint8_t supports_x16, uint8_t row_cycles, 
	uint8_t col_cycles, uint8_t page_bits, uint32_t bytes_to_read, uint8_t silent, uint8_t* buffer)
{
	setNandIOValue(0x0000);
	setNandIODir(OUT);
	onfiCommand(ONFI_CMD_READ_CYCLE_1);
	onfiAddressStart();
	/* column addressing */
	for(uint8_t i=0; i<col_cycles; ++i){
		onfiAddress(0x00);
	}

	/* row addressing */
	uint64_t row_address = page;
	row_address |= (((uint64_t)block)<<page_bits);

	for(uint8_t i=0; i<row_cycles; i++) {
		onfiAddress((uint8_t)(row_address>>(i*8)));
	}

	onfiAddressEnd();
	onfiCommand(ONFI_CMD_READ_CYCLE_2);
	onfiDataOutStart();
	while(!getNandRB());

	bytes_to_read = bytes_to_read / (supports_x16 + 1);
	
	for (uint16_t i = 0; i < bytes_to_read; ++i)
	{
		uint8_t temp_high = 0;
		uint8_t temp_low = 0;
		
		onfiDataOut(&temp_low, &temp_high);
		if(buffer != NULL){
			if(supports_x16){
				buffer[i*2+1] = temp_high;
				buffer[i*2] = temp_low;
			}else{
				buffer[i] = temp_low;
			}
		}
		if(!silent){
			dumpByte(temp_low);
			if(supports_x16){
				dumpByte(temp_high);
			}
		}
	}
	onfiDataOutEnd();
}

static inline uint8_t ecc_repetition_decode(uint8_t rep1, uint8_t rep2, uint8_t rep3)
{
	uint8_t result=0;
	for(uint8_t i = 0; i<8; i++){
		//count 1 and decide if majority:
		uint8_t cnt = (rep1&(1<<i))+(rep2&(1<<i))+(rep3&(1<<i));
		if(cnt>=2){
			result |= (1<<i);
		}
	}
	return result;
}

static inline uint32_t onfiVerifiedDumpPage(uint32_t block, uint32_t page, uint8_t supports_x16, uint8_t row_cycles, 
	uint8_t col_cycles, uint8_t page_bits, uint32_t bytes_to_read)
{
	uint8_t copy_1[bytes_to_read];
	uint8_t copy_2[bytes_to_read];
	uint8_t copy_3[bytes_to_read];	
	onfiDumpPage(block, page, supports_x16, row_cycles, col_cycles, page_bits, bytes_to_read,1, copy_1);	
	onfiDumpPage(block, page, supports_x16, row_cycles, col_cycles, page_bits, bytes_to_read,1, copy_2);
	onfiDumpPage(block, page, supports_x16, row_cycles, col_cycles, page_bits, bytes_to_read,1, copy_3);

	for(uint32_t i= 0; i<bytes_to_read; i++){
		dumpByte(ecc_repetition_decode(copy_1[i],copy_2[i],copy_3[i]));
	}
}
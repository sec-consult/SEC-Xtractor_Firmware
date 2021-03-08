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
#include "../function_modules/nand.c"

/**
* @brief Erases the whole Nand Flash
*/
static void cmdEraseNand()
{
	if(isOnfiParamConfigured()) {
		uartWriteString(NL "Erasing Nand: " NL);
		eraseNand();
		uartWriteString(NL "Operations finished successfully." NL);
	}
}

/**
* @brief	Writes a test pattern to the whole Nand
*/
static void cmdWriteNand()
{
	uint8_t write_through_ecc = 0;

	uartWriteString(NL "Write through ECC Bytes? [Y/n]: " NL);
	while(!uartHasInput());
	if(uartGetInput() != 'n') {
		uartWriteString("Writing through ECC!"NL);
		write_through_ecc = 1;
	}else{
		uartWriteString("Skipping ECC!"NL);
		write_through_ecc = 0;	
	}
	writeNand(write_through_ecc);
	uartWriteString(NL "Operations finished successfully." NL);
}

/**
 * @brief Function to dump NAND flash memory.
 */
static void cmdDumpNand(char *arguments)
{
	uartWriteString(NL "Dump with ECC Bytes? [Y/n]: " NL);
	while(!uartHasInput());
	uint8_t dump_with_ecc = uartGetInput() != 'n';
	uartWriteString(NL "Verify read operations? [Y/n]: " NL);
	while(!uartHasInput());
	uint8_t dump_verify = uartGetInput() != 'n';
	if(dump_with_ecc){
		uartWriteString("Dumping with ECC ");
	}else{
		uartWriteString("Dumping without ECC ");
	}
	if(dump_verify){
		uartWriteString("with verification!"NL);
	}else{
		uartWriteString("without verification!"NL);
	}
	dumpNand(dump_with_ecc, dump_verify);
}

/**
 * @brief Function to return the chip ID and the ONFI string of a NAND flash memory.
 */
static inline void cmdInfoNand(char *arguments)
{
	onfiPrintParams();
}


/**
 *	@brief	configure the onfi params
 */
static void cmdConfigOnfi()
{
	char signature[4];
	
	uartWriteString("Configuring device with ONFI parameters"NL);

	int res = configOnfi();
	if(res & 2){
		uartWriteString("ONFI configuration failed! Please retry..."NL);
		return;
	}
	if(res & 1){
		uartWriteString(NL"ONFI parameter page copys do not match. Further operation might be prone to errors!"NL);
	}

	uartWriteString("ONFI configuration successful!"NL);
	onfiPrintParams();
}

void cmdDelayNand(char *arguments)
{
	long delay;
	if (*arguments != '\0')
	{
		delay = atoi(arguments);
		setDelayNand(delay);
	}
	uartprintf("Nand chip delay: %ld iterations" NL, delay);
}

/**
 *	@brief	configure the onfi params
 */
static void cmdConfigOnfiManual()
{
	onfi_infos_t onfi_infos;
	uint8_t buffer[8];
	uint8_t i = 0;
	uartWriteString("Manually configuring device with ONFI parameters"NL);
	
	uartWriteString("How many LUNs exist?"NL);
	do {
		while(!uartHasInput());
		buffer[i] = uartGetInput();
		uartWriteChar(buffer[i]);
		i++;
	}while(buffer[i-1] != '\n' && i<sizeof(buffer));
	buffer[i-1] = 0;
	onfi_infos.params.logical_units = atoi(buffer);
	i=0;

	uartWriteString("How many Blocks per LUN?"NL);
	do {
		while(!uartHasInput());
		buffer[i] = uartGetInput();
		uartWriteChar(buffer[i]);
		i++;
	}while(buffer[i-1] != '\n' && i<sizeof(buffer));
	buffer[i-1] = 0;
	onfi_infos.params.block_per_logic_unit = atoi(buffer);
	i=0;

	uartWriteString("How many Pages per Block?"NL);
	do {
		while(!uartHasInput());
		buffer[i] = uartGetInput();
		uartWriteChar(buffer[i]);
		i++;
	}while(buffer[i-1] != '\n' && i<sizeof(buffer));
	buffer[i-1] = 0;
	onfi_infos.params.pages_per_block = atoi(buffer);
	i=0;

	uartWriteString("How many Bytes per Page without ECC?"NL);
	do {
		while(!uartHasInput());
		buffer[i] = uartGetInput();
		uartWriteChar(buffer[i]);
		i++;
	}while(buffer[i-1] != '\n' && i<sizeof(buffer));
	buffer[i-1] = 0;
	onfi_infos.params.bytes_per_page = atoi(buffer);
	i=0;

	uartWriteString("How many Bytes per Spare Page?"NL);
	do {
		while(!uartHasInput());
		buffer[i] = uartGetInput();
		uartWriteChar(buffer[i]);
		i++;
	}while(buffer[i-1] != '\n' && i<sizeof(buffer));
	buffer[i-1] = 0;
	onfi_infos.params.spare_bytes_per_page = atoi(buffer);
	i=0;

	uartWriteString("Number of Row Cycles?"NL);
	do {
		while(!uartHasInput());
		buffer[i] = uartGetInput();
		uartWriteChar(buffer[i]);
		i++;
	}while(buffer[i-1] != '\n' && i<sizeof(buffer));
	buffer[i-1] = 0;
	onfi_infos.params.address_cycles = atoi(buffer)&0x0f;
	i=0;

	uartWriteString("Number of Column Cycles?"NL);
	do {
		while(!uartHasInput());
		buffer[i] = uartGetInput();
		uartWriteChar(buffer[i]);
		i++;
	}while(buffer[i-1] != '\n' && i<sizeof(buffer));
	buffer[i-1] = 0;
	onfi_infos.params.address_cycles |= (atoi(buffer)<<4);
	i=0;

	uartGetInput();
	uartWriteString("Supports x16 [Y/n]: "NL);
	while(!uartHasInput());
	if(uartGetInput() != 'n'){
		onfi_infos.params.feature_support = 0x0001;
	}

	//Let ONFI checks pass by writing ONFI in Config
	memcpy(onfi_infos.params.signature, "ONFI", 4);

	uartWriteString("ONFI Manual Configuration done!"NL);
	onfiPrintParams(&onfi_infos);
}

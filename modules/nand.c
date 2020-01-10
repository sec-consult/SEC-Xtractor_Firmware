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
#include "../secxtractor.h"
#include "../hal.h"

onfi_infos_t onfi_infos;

/**
* @brief	inits ONFI params to zero
*/
static void initOnfiParam()
{
	memset(&onfi_infos.params, 0, sizeof(onfi_param_page_t));	
}

/**
* @brief	checks if the ONFI Parameters are initialized, otherwise prints error message
* @returns	0	error
* @returns	1	ONFI Parameters existent
*/
static uint8_t isOnfiParamConfigured()
{
	if(memcmp(&onfi_infos.params.signature, "ONFI", 4) == 0){
		return 1;
	}
	uartWriteString(NL"ONFI Parameters unknown. Try to run 'config onfi' first"NL);
	return 0;
}


/**
* @brief Erases the whole Nand Flash
*/
static void cmdEraseNand()
{
	if(isOnfiParamConfigured()) {
		uartWriteString(NL "Erasing Nand: " NL);
		nandInit();
		nandReset();
		onfiEraseAll(&onfi_infos.params,1);
		uartWriteString(NL "Operations finished successfully." NL);
	}
}

/**
* @brief	Writes a test pattern to the whole Nand
*/
static void cmdWriteNand()
{
	if(isOnfiParamConfigured()){
		nandInit();
		nandReset();
		uartWriteString(NL "Write through ECC Bytes? [Y/n]: " NL);
		while(!uartHasInput());
		if(uartGetInput() != 'n'){
			uartWriteString("Writing through ECC!"NL);
			onfiWriteData(&onfi_infos.params, 0xbeef, 1);
		}else{
			uartWriteString("Skipping ECC!"NL);
			onfiWriteData(&onfi_infos.params, 0xbeef, 0);
		}
		uartWriteString(NL "Operations finished successfully." NL);
	}
}

/**
 * @brief Function to dump NAND flash memory.
 */
static void cmdDumpNand(char *arguments)
{
	if(isOnfiParamConfigured()){
		nandInit();
		nandReset();
		uartWriteString(NL "Dump with ECC Bytes? [Y/n]: " NL);
		while(!uartHasInput());
		uint8_t dump_ecc = uartGetInput() != 'n';
		
		uartWriteString(NL "Verify read operations? [Y/n]: " NL);
		while(!uartHasInput());
		uint8_t verify_read = uartGetInput() != 'n';
		
		if(dump_ecc){
			uartWriteString("Dumping with ECC ");
		}else{
			uartWriteString("Dumping without ECC ");
		}

		if(verify_read){
			uartWriteString("with verification!"NL);
		}else{
			uartWriteString("without verification!"NL);
		}

		onfiReadData(&onfi_infos.params, dump_ecc, verify_read);
	}
}

/**
* @brief	Prints some ONFI Parameters to the terminal
*/
static inline void onfiPrintParams(onfi_infos_t *infos)
{
	onfi_param_page_t *params = &infos->params;
	onfi_feature_description_t *ft = &onfi_feature_descriptions[0];

	uartprintf("Chip ID:\t\t\t%02x %02x %02x %02x %02x"NL, 
		(unsigned char) infos->chipId[0], (unsigned char) infos->chipId[1], 
		(unsigned char) infos->chipId[2], (unsigned char) infos->chipId[3], 
		(unsigned char) infos->chipId[4]);
	uartprintf("Manufacturer:\t\t\t%c%c%c%c%c%c%c%c%c%c%c%c"NL,
		params->vendor[0],params->vendor[1],params->vendor[2],params->vendor[3],params->vendor[4],
		params->vendor[5],params->vendor[6],params->vendor[7],params->vendor[8],params->vendor[9],
		params->vendor[10],params->vendor[11]);
	uartprintf("Model:\t\t\t\t%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"NL,
		params->model[0],params->model[1],params->model[2],params->model[3],params->model[4],
		params->model[5],params->model[6],params->model[7],params->model[8],params->model[9],
		params->model[10],params->model[11],params->model[12],params->model[13],params->model[14],
		params->model[15],params->model[16],params->model[17],params->model[18],params->model[19]);
	uartprintf("Date Code:\t\t\t0x%04x"NL, params->date);
	uartprintf("ONFI Revision:\t\t\t0x%04x"NL, params->rev_num);
	uartprintf("Features supported:\t\t0x%04x"NL, params->feature_support);

	while(ft->mask != 0){
		if(ft->mask & params->feature_support){
			uartprintf("\t\t\t\t* %s (0x%x)"NL, ft->description, ft->mask);
		}
		ft++;
	}

	uartprintf("Optional commands supported:\t0x%04x"NL, params->opt_cmd_support);
	uartprintf("Bytes per Page:\t\t\t%"PRIu32 NL, params->bytes_per_page);
	uartprintf("Spare bytes per Page:\t\t%u"NL, params->spare_bytes_per_page);
	uartprintf("Pages per Block:\t\t%"PRIu32 NL, params->pages_per_block);
	uartprintf("Blocks per LUN:\t\t\t%"PRIu32 NL, params->block_per_logic_unit);
	uartprintf("Number of LUNs:\t\t\t%u"NL, params->logical_units);
}


/**
 * @brief Function to return the chip ID and the ONFI string of a NAND flash memory.
 */
static inline void cmdInfoNand(char *arguments)
{
	if(isOnfiParamConfigured()){
		onfiPrintParams(&onfi_infos);
	}
}


/**
 *	@brief	configure the onfi params
 */
static void cmdConfigOnfi()
{
	char signature[4];
	
	uartWriteString("Configuring device with ONFI parameters"NL);
	nandInit();
	nandReset();
	nandReadONFI(signature);

	if(memcmp(signature, "ONFI", 4) != 0){
		uartWriteString("NAND chip did not report to be ONFI compatible"NL);
		return;
	}

	nandReadChipID(onfi_infos.chipId);

	int res = onfiReadAndCheckParameters(&onfi_infos.params);
	
	if(res & 2){
		uartWriteString("ONFI configuration failed! Please retry..."NL);
		return;
	}

	if(res & 1){
		uartWriteString(NL"ONFI parameter page copys do not match. Further operation might be prone to errors!"NL);
	}

	uartWriteString("ONFI configuration successful!"NL);
	cmdInfoNand("");
}

void cmdDelayNand(char *arguments)
{

	if (*arguments != '\0')
	{
		nandDelayTime = atoi(arguments);
	}
	uartprintf("Nand chip delay: %ld iterations" NL, nandDelayTime);
}
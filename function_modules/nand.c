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

#include "low_level/nand.c"

onfi_infos_t onfi_infos;

/**
* @brief	inits ONFI params to zero
*/
static void initOnfiParam()
{
	memset(&onfi_infos.params, 0, sizeof(onfi_param_page_t));	
}

static void manualSetOnfiParam(onfi_infos_t *manual)
{
	memset(&onfi_infos, 0, sizeof(onfi_param_page_t));
	memcpy(&onfi_infos, manual, sizeof(onfi_infos_t));
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
static void eraseNand()
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
* @param    write_trhough_ecc   1 = Write through ECC
*                               0 = Do not write through ECC
*/
static void writeNand(uint8_t write_through_ecc)
{
	if(isOnfiParamConfigured()){
		nandInit();
		nandReset();
		if(write_through_ecc){
			onfiWriteData(&onfi_infos.params, 0xbeef, 1);
		}else{
			onfiWriteData(&onfi_infos.params, 0xbeef, 0);
		}
	}
}

/**
 * @brief Function to dump NAND flash memory.
 * @param dump_with_ecc     1= includes spare pages
 * @param dump_verify       1=verify read data
 */
static void dumpNand(uint8_t dump_with_ecc, uint8_t dump_verify)
{
	if(isOnfiParamConfigured()){
		nandInit();
		nandReset();
		onfiReadData(&onfi_infos.params, dump_with_ecc, dump_verify);
	}
}

/**
* @brief	Prints some ONFI Parameters to the terminal
*/
static inline void onfiPrintParams()
{
	onfi_infos_t* infos = &onfi_infos;
    if(isOnfiParamConfigured()){
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
}

/**
 *	@brief	configure the onfi params
 *  @return     1  =   Config failed / Not ONFI compatible
 *  @return     2  =   Param Page copies do not match
 */
static int configOnfi()
{
	char signature[4];
	nandInit();
	nandReset();
	nandReadChipID(onfi_infos.chipId);

	return onfiReadAndCheckParameters(&onfi_infos.params);
}

void setDelayNand(long delay)
{
	nandDelayTime = delay;
}

/**
*@brief				Reads Data from NAND
*@param	params		Pointer to the ONFI parameter page
*@param read_ecc	If set to 1, will read with ECC block, otherwise ECC block will be skipped
*/
static inline void onfiReadData(onfi_param_page_t *params, uint8_t read_ecc, uint8_t verify_read_iterations)
{
uint8_t supports_x16 = params->feature_support & ONFI_FEATURE_16_BIT_BUS;
	uint8_t row_cycles = params->address_cycles &0x0f;
	uint8_t col_cycles = params->address_cycles >> 4;
	uint8_t page_bits = 255; //Loop counts one bit too much
	uint32_t block, page;
	//int i;
	uint8_t abort = 0;
	uint32_t reference_checksum, test_checksum, test_checksum2;
	uint32_t bytes_to_read = params->bytes_per_page + (read_ecc * params->spare_bytes_per_page);

	for(uint32_t tmp_pages_per_block = params->pages_per_block;\
			tmp_pages_per_block != 0; tmp_pages_per_block>>=1){
		page_bits++;
	}

	dumpStart();

	for(block = 0; block < params->block_per_logic_unit; block++){
		if(isOperationCanceled() || abort){
			break;
		}

		for(page = 0; page < params->pages_per_block && !abort; page++){
			if(verify_read_iterations){
				onfiVerifiedDumpPage(block, page, supports_x16, row_cycles, col_cycles, page_bits, bytes_to_read);
			}else{
				onfiDumpPage(block, page, supports_x16, row_cycles, col_cycles, page_bits, bytes_to_read,0, NULL);
			}
		}
	}

	dumpEnd();
	
	uartWriteString(NL "reading done" NL);
}
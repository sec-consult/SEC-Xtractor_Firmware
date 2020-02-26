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
			onfiWriteData(&onfi_infos.params, 0xaaaa, 1);
		}else{
			uartWriteString("Skipping ECC!"NL);
			onfiWriteData(&onfi_infos.params, 0xaaaa, 0);
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
		uartWriteString(NL "Dump with ECC Bytes? [Y/n]: " NL);
		while(!uartHasInput());
		uint8_t dump_ecc = uartGetInput() != 'n';
		
		#ifdef XTRACTOR_ARCH_ARM
		uartWriteString(NL "Verify read operations? [Y/n]: " NL);
		while(!uartHasInput());
		uint8_t verify_read = uartGetInput() != 'n';
		#else
		uint8_t verify_read = 0;
		#endif
		
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
		nandInit();
		nandReset();
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
	uartprintf("Timing mode supported:\t\t0x%04x"NL, params->timing_mode_support);
	for(uint8_t i=0; i<=5; i++){
		if(params->timing_mode_support & (1<<i)){
			uartprintf("\t\t\t\t* Timing Mode %u"NL,i);
		}
	}
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

/**
 *	@brief	configure the onfi params
 */
static void cmdConfigOnfiManual()
{
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

void cmdDelayNand(char *arguments)
{

	if (*arguments != '\0')
	{
		nandDelayTime = atoi(arguments);
	}
	uartprintf("Nand chip delay: %ld iterations" NL, nandDelayTime);
}

/**
*@brief	writes a command byte to the nand according to ONFI standard
*/
static inline void onfiCommand(uint8_t command)
{
	setNandIOValue(0x0000);
	setNandIODir(OUT);
	// setPinValue(NAND_CTRL_PORT.ce, LOW);
	// setPinValue(NAND_CTRL_PORT.cle, LOW);
	// setPinValue(NAND_CTRL_PORT.ale, LOW);
	setPinValue(NAND_CTRL_PORT.we, HIGH);
	setPinValue(NAND_CTRL_PORT.ce, LOW);
	setPinValue(NAND_CTRL_PORT.cle, HIGH);
	setPinValue(NAND_CTRL_PORT.we, LOW);
	setNandIOValue(command);
	delay_ns(T_WP);
	setPinValue(NAND_CTRL_PORT.we, HIGH);
	delay_ns(T_CLH);
	setPinValue(NAND_CTRL_PORT.cle, LOW);
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
				onfiDataIn((uint8_t) pattern,(uint8_t) (pattern >> 8));
			}else{
				if(i%2){
					onfiDataIn((uint8_t) pattern, 0);
				}else{
					onfiDataIn((uint8_t) pattern>>8, 0);
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
			if(supports_x16){
				dumpByte(temp_high);
			}
			dumpByte(temp_low);
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
	uint8_t page_1[bytes_to_read];
	uint8_t page_2[bytes_to_read];
	uint8_t page_3[bytes_to_read];	
	onfiDumpPage(block, page, supports_x16, row_cycles, col_cycles, page_bits, bytes_to_read,1, page_1);	
	onfiDumpPage(block, page, supports_x16, row_cycles, col_cycles, page_bits, bytes_to_read,1, page_2);
	onfiDumpPage(block, page, supports_x16, row_cycles, col_cycles, page_bits, bytes_to_read,1, page_3);

	for(uint32_t i= 0; i<bytes_to_read; i++){
		dumpByte(ecc_repetition_decode(page_1[i],page_2[i],page_3[i]));
	}
}


// static inline uint32_t onfiDumpPage(uint32_t block, uint32_t page, uint8_t supports_x16, uint8_t row_cycles, 
// 	uint8_t col_cycles, uint8_t page_bits, uint32_t bytes_to_read, uint8_t silent, uint8_t* buffer)
// {
// 	uint32_t checksum = 0;

// 	setNandIOValue(0x0000);
// 	setNandIODir(OUT);

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

// 	/* end of sending command, start reading the whole page */
// 	onfiDataOutStart();
// 	while(!getNandRB());

// 	bytes_to_read = bytes_to_read / (supports_x16 + 1);
	
// 	for (uint16_t i = 0; i < bytes_to_read; ++i)
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
		
// 		checksum += (((uint16_t)temp_high) << 8) + temp_low;
// 	}
// 	onfiDataOutEnd();
// 	delay_ns(100);
// 	//_delay_32mhz_cycles(nandDelayTime);

// 	return checksum;
// }

/**
*@brief				Writes a pattern throughout the whole NAND
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
	int i;
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
			//reference_checksum = 

			if(verify_read_iterations){
				onfiVerifiedDumpPage(block, page, supports_x16, row_cycles, col_cycles, page_bits, bytes_to_read);
			}else{
				onfiDumpPage(block, page, supports_x16, row_cycles, col_cycles, page_bits, bytes_to_read, 0, NULL);
			}

			// for(i=0; i<verify_read_iterations; i++){
			// 	test_checksum = onfiDumpPage(block, page, supports_x16, row_cycles, col_cycles, page_bits, bytes_to_read, 1);
			// 	test_checksum2 = onfiDumpPage(block, page, supports_x16, row_cycles, col_cycles, page_bits, bytes_to_read, 1);
			// 	if(reference_checksum != test_checksum && reference_checksum != test_checksum2 && test_checksum2!=test_checksum){
			// 		uartWriteString(NL OUTPUT_BOUNDARY_THICK);
			// 		uartprintf("Error during verification of block %ld page %ld (checksum mismatch '%08lx' != '%08lx')"NL, 
			// 			block, page, reference_checksum, test_checksum);
			// 		abort = 1;
			// 		break;
			// 	}
			// }
		}
	}

	dumpEnd();
	
	uartWriteString(NL "reading done" NL);
}
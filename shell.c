/**
 * @file shell.c
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
#include "hal.h"
#include "secxtractor.h"

#include "modules/jtag.c"
#include "modules/nand.c"
#include "modules/nor.c"
#include "modules/spi.c"
#include "modules/uart_scanner.c"

#define HELPNL NL "    "

/* shell variables */
static char commandinput[SHELL_LINE_MAX_LENGTH];
static uint8_t commandpointer = 0;

static void showHelp(char* prefix, uint8_t details, uint8_t completeCommandInput);


/**
 * @brief General help output for usage.
 **/
static void cmdHelp(char *arguments)
{
	uartWriteString(NL);
	uartWriteString(GLOBAL_VERSION_STRING NL);
	uartWriteString(
		"The SEC Xtractor is designed as hardware testing tool by SEC Consult Unternehmensberatung GmbH." NL
		"Use `help detail' to find out more about all commands." NL
		NL);
	showHelp("", 0, 0);
}

/**
 * @brief Detailed help output for all functions.
 **/
static void cmdHelpDetail(char *arguments)
{
	uartWriteString(NL);
	uartWriteString(GLOBAL_VERSION_STRING NL);
	uartWriteString(
		"The SEC Xtractor is designed as hardware testing tool by SEC Consult Unternehmensberatung GmbH." NL
		NL);
	showHelp("", 1, 0);
}

void cmdDebug(char *arguments)
{
	debug = ~debug;
	if (debug)
	{
		uartWriteString("DEBUG ON" NL);
	}
	else
	{
		uartWriteString("DEBUG OFF" NL);
	}
}

void cmdReset(char *arguments)
{
	resetDevice();
}

void cmdFastdump(char *arguments)
{
	if(arguments[0] == '1'){
		dumpFast = 1;
		uartWriteString("FASTDUMP ON" NL);
	}else{
		dumpFast = 0;
		uartWriteString("FASTDUMP OFF" NL);
	}
}

shell_command_t commands[] = {
	/* generic */
	SHELL_COMMAND("help detail", cmdHelpDetail, "", 
		"Prints the detailed help page."),
	SHELL_COMMAND("help", cmdHelp, "", 
		"Prints the short help page."),
	SHELL_COMMAND("debug", cmdDebug, "", ""),
	SHELL_COMMAND("reset", cmdReset, "", ""),
	SHELL_COMMAND("fastdump", cmdFastdump, "<0|1>", 
		"Enable or disable fast dumping (skip formating and dump 1K in every line)."),
	/* JTAG */
	SHELL_COMMAND("pattern scan single", cmdPatternScanSingle, "", 
		"Probe for a present JTAG port by using the pre-selected JTAG pin configuration"),
	SHELL_COMMAND("pattern scan", cmdPatternScan, "", 
		"Scans for a present JTAG port on all activated A pins of the SEC Xtractor."),
	SHELL_COMMAND("idcode scan", cmdIdcodeScan, "", 
		"Assumes IDCODE is default DR on reset. Ignores TDI." HELPNL
		"Sets TAP state to DR_SHIFT and prints TDO to console" HELPNL
		"when TDO appears active. Human examination required to" HELPNL
		"determine if actual IDCODE is present. Run several" HELPNL
		"times to check for consistancy or compare against" HELPNL
		"active tdo lines found with loopback test."),
	SHELL_COMMAND("bypass scan", cmdBypassScan, "", 
		"Assumes BYPASS is default DR on reset. Ignores TMS and" NL
		"shifts pattern[] through TDI/TDO using TCK for clock." NL),
	SHELL_COMMAND("boundary scan", cmdBoundaryScan, "",
		"Checks code defined tdo for 4000+ bits."),
	SHELL_COMMAND("pattern set", cmdPatternSet, "[numbers_from_1_to_32]", ""),
	SHELL_COMMAND("pinlen set", cmdPinlenSet, "[number_in_the_range_from_1_to_32]", 
		"Set length of pins for brute force JTAG."),
	SHELL_COMMAND("tck set", cmdTckSet, "[number_in_the_range_from_1_to_32]", ""),
	SHELL_COMMAND("tms set", cmdTmsSet, "[number_in_the_range_from_1_to_32]", ""),
	SHELL_COMMAND("tdi set", cmdTdiSet, "[number_in_the_range_from_1_to_32]", ""),
	SHELL_COMMAND("tdo set", cmdTdoSet, "[number_in_the_range_from_1_to_32]", ""),
	SHELL_COMMAND("trst set", cmdTrstSet, "[number_in_the_range_from_1_to_32]", ""),
	SHELL_COMMAND("loopback check", cmdLoopbackCheck, "", ""),
	SHELL_COMMAND("ir len set", cmdIrLenSet, "[number_in_the_range_from_1_to_40]", ""),
	SHELL_COMMAND("flashsize set", cmdFlashsizeSet, "", ""),
	SHELL_COMMAND("delay", cmdDelay, "[+/-]", 
		"No arguments: Slows down the JTAG scan." HELPNL
		"With arguments + or -: Reduces or increases the scan delay by 1000us"),
	SHELL_COMMAND("irenum", cmdIrEnum, "", 
		"Sets every possible Instruction Register and then checks the output of the Data Register."),
	/* NAND */
	SHELL_COMMAND("info nand", cmdInfoNand, "", 
		"Displays information about the NAND memory gathered from the ONFI parameter pages."),
	SHELL_COMMAND("write nand", cmdWriteNand, "", 
		"Writes 0xbeef into the entire NAND memory. Choices can be made to include or exclude ECC bytes."),
	SHELL_COMMAND("dump nand", cmdDumpNand, "", 
		"Dumps the content of the NAND memory. ECC bytes can be included or excluded."),
	SHELL_COMMAND("config onfi", cmdConfigOnfi, "", 
		"Reads the ONFI parameter page of the NAND memory. These values are required for further communication with the flash memory."),
	SHELL_COMMAND("erase nand", cmdEraseNand, "", 
		"Erases all content from the NAND memory."),
	/* NOR */
	SHELL_COMMAND("dump nor", cmdDumpNor, "[endianess (1/0)] [enable word mode (1/0)]", ""),
	//SHELL_COMMAND("write nor pattern", cmdNorPattern),
	/* SPI */
	SHELL_COMMAND("spi dump", cmdSpiDump, "", ""),
	/* UART */
	SHELL_COMMAND("uart scanner", cmdUartScanner, "<pinlen>", 
		"Scans for toggling pins which are hold high during idle times. This indicates the presence of a UART bus."),
	END_OF_COMMANDS()
};

void shellInit(void)
{
	memset(commandinput, 0, SHELL_LINE_MAX_LENGTH);
	uartWriteString(GLOBAL_VERSION_STRING NL);
	uartWriteString(SHELL_STRING);
}

static inline void executeCommand(void)
{
	shell_command_t *commandPtr = &commands[0];
	int tmp;

	cancelCurrentOperation = 0;

	sevensegShowProgressBar();

	// remove trailing spaces
	while (commandpointer > 0 && commandinput[commandpointer] == ' ')
	{
		commandpointer--;
	}

	commandinput[commandpointer] = '\0';

	if(commandinput[0] == '\0')
	{
		// no command -> nothing to do
		return;
	}

	while (!IS_END_OF_COMMANDS(commandPtr))
	{
		tmp = strlen(commandPtr->commandPrefix);

		if (strncmp(commandinput, commandPtr->commandPrefix, tmp) == 0)
		{
			// remove spaces between command and arguments
			while (commandinput[tmp] == ' ')
			{
				tmp++;
			}
			commandPtr->callback(&commandinput[tmp]);
			return;
		}
		commandPtr++;
	}

	uartprintf("ERROR: Unknown Command '%s', try 'help'" NL, commandinput);	
}

/**
 * @brief Main shell of the tool. 
 * @param character The input character that gets processed.
 */
static void shell(char character)
{
	if (commandpointer >= SHELL_LINE_MAX_LENGTH)
	{
		return;
	}
	switch (character)
	{
	case CHAR_BACKSPACE:	// backspace
	case CHAR_DEL:		 	// del (putty)
		if (commandpointer != 0) //delete one char in memory
		{
			uartWriteChar(CHAR_BACKSPACE);
			uartWriteChar(' ');
			uartWriteChar(CHAR_BACKSPACE);
			commandpointer--;
			commandinput[commandpointer] = '\0';
		}
		break;
	case CHAR_CR:
	case CHAR_LF:
		uartWriteString(NL);
		commandinput[commandpointer] = '\0'; // NUL-terminate string
		executeCommand();
		uartWriteString(SHELL_STRING);
		commandpointer = 0;
		commandinput[0] = '\0';
		break;
	case CHAR_BREAK:
		uartWriteString(NL SHELL_STRING);
		commandpointer = 0;
		commandinput[0] = '\0';
		break;
	case CHAR_TAB:
		commandinput[commandpointer] = '\0'; // NUL-terminate string
		uartWriteString(NL);
		showHelp(commandinput, 0, 1);
		uartWriteString(SHELL_STRING);
		uartWriteString(commandinput);
		break;
	default: //default char
		if(character < 32 || character >= 127){
			// non-ASCII or other non-printable
			uartWriteChar(CHAR_BELL);
		}else{
			uartWriteChar(character);
			commandinput[commandpointer] = character;
			commandpointer++;
		}
		break;
	}
}

static inline int countSameChars(const char *s1, const char *s2, int length){
	int i = 0;
	while(i < length && s2[i] != '\0' && s2[i] != '\0' && s1[i] == s2[i]){
		i++;
	}
	return i;
}

static void showHelp(char* prefix, uint8_t details, uint8_t completeCommandInput)
{
	shell_command_t *commandPtr = &commands[0];
	uint8_t prefixEndPointer = strlen(prefix);
	const char *matchingStrPtr = NULL;
	int matchingStrLen = 0;
	uint8_t matchingCommandCount = 0;

	// remove trailing spaces
	while (prefixEndPointer > 0 && prefix[prefixEndPointer] == ' ')
	{
		prefixEndPointer--;
	}

	while (!IS_END_OF_COMMANDS(commandPtr))
	{
		if (strncmp(prefix, commandPtr->commandPrefix, prefixEndPointer) == 0)
		{
			matchingCommandCount++;
			if(matchingCommandCount == 1){
				matchingStrPtr = commandPtr->commandPrefix;
				matchingStrLen = strlen(commandPtr->commandPrefix);
			}else{
				matchingStrLen = countSameChars(matchingStrPtr, commandPtr->commandPrefix, matchingStrLen);
			}
			uartprintf("%s %s" NL, commandPtr->commandPrefix, commandPtr->usage);
			if(details){
				uartprintf("    %s" NL, commandPtr->helptext);
			}
		}
		commandPtr++;
	}

	if(completeCommandInput && matchingStrLen > commandpointer){
		strncpy(commandinput, matchingStrPtr, matchingStrLen);
		commandpointer = matchingStrLen;
		if(matchingCommandCount == 1){
			commandinput[commandpointer++] = ' ';
		}
		commandinput[commandpointer] = '\0';
	}
}
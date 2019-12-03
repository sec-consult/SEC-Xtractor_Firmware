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

/* shell variables */
static char commandinput[SHELL_LINE_MAX_LENGTH];
static uint8_t commandpointer = 0;

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
		NL
		" help (prints this output)" NL
		" help detail (prints an extended output)" NL
		" pattern scan single" NL
		" pattern scan" NL
		" pattern set [numbers_from_1_to_32] (specify the JTAG test pattern)" NL
		" pinlen set [number_in_the_range_from_1_to_32] (specify the count of A-pins that are used for brute-force actions)" NL
		" tck set [number_in_the_range_from_1_to_32]" NL
		" tms set [number_in_the_range_from_1_to_32]" NL
		" tdi set [number_in_the_range_from_1_to_32]" NL
		" tdo set [number_in_the_range_from_1_to_32]" NL
		" trst set [number_in_the_range_from_1_to_32]" NL
		" ir len set [number_in_the_range_from_1_to_40]" NL
		" loopback check" NL
		" idcode scan" NL
		" irenum" NL
		" loopback check" NL
		" boundary scan" NL
		" delay" NL
		" debug" NL
		" delay [+/-]" NL
		" bypass scan" NL
		" dump nor [endianess (1/0)] [enable word mode (1/0)]" NL
		" dump nand" NL
		" info nand" NL
		" write nand" NL
		" erase nand" NL
		" config onfi" NL
		" uart scanner (pinlen)" NL);
}

/**
 * @brief Detailed help output for all functions.
 **/
static void cmdHelpDetail(char *arguments)
{
	uartWriteString(NL
					"help: help" NL
					"    Prints the short help page." NL
					"help detail: help detail" NL
					"    Prints this detailed help page." NL
					"pattern scan single: pattern scan single" NL
					"    Probe for a present JTAG port by using the pre-selected JTAG pin configuration." NL
					"pattern scan" NL
					"    Scans for a present JTAG port on all activated A pins of the SEC Xtractor." NL
					"idcode scan" NL
					"    Assumes IDCODE is default DR on reset. Ignores TDI."NL
					"    Sets TAP state to DR_SHIFT and prints TDO to console"NL
					"    when TDO appears active. Human examination required to"NL
					"    determine if actual IDCODE is present. Run several"NL
					"    times to check for consistancy or compare against"NL
					"    active tdo lines found with loopback test."NL
					"bypass scan" NL
					"    Assumes BYPASS is default DR on reset. Ignores TMS and" NL
					"    shifts pattern[] through TDI/TDO using TCK for clock." NL
					"boundary scan" NL
					"    Checks code defined tdo for 4000+ bits." NL
					"irenum" NL
					"    Sets every possible Instruction Register and then checks the output of the Data Register." NL
					"delay" NL
					"    Will slow down the JTAG scan"
					"delay [+/-]" NL
					"    Reduces or increases the scan delay by 1000us" NL
					"pinlen set" NL
					"    Set lenght of pins for brute force JTAG" NL
					"uart scanner" NL
					"    Scans for toggling pins which are hold high during idle times. This indicates the presence of a UART bus." NL
					"config onfi" NL
					"    Reads the ONFI parameter page of the NAND memory. These values are required for further communication with the flash memory." NL
					"info nand" NL
					"    Displays information about the NAND memory gathered from the ONFI parameter pages." NL
					"erase nand" NL
					"    Erases all content from the NAND memory." NL
					"write nand" NL
					"    Writes 0xbeef into the entire NAND memory. Choices can be made to include or exclude ECC bytes." NL
					"dump nand" NL
					"    Dumps the content of the NAND memory. ECC bytes can be included or excluded." NL
					"dump nor" NL
					"    Dumps the content of the NOR memory." NL
	);
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

shell_command_t commands[] = {
	/* generic */
	SHELL_COMMAND("help detail", cmdHelpDetail),
	SHELL_COMMAND("help", cmdHelp),
	SHELL_COMMAND("debug", cmdDebug),
	SHELL_COMMAND("reset", cmdReset),
	/* JTAG */
	SHELL_COMMAND("pattern scan single", cmdPatternScanSingle),
	SHELL_COMMAND("pattern scan", cmdPatternScan),
	SHELL_COMMAND("idcode scan", cmdIdcodeScan),
	SHELL_COMMAND("bypass scan", cmdBypassScan),
	SHELL_COMMAND("boundary scan", cmdBoundaryScan),
	SHELL_COMMAND("pattern set", cmdPatternSet),
	SHELL_COMMAND("pinlen set", cmdPinlenSet),
	SHELL_COMMAND("tck set", cmdTckSet),
	SHELL_COMMAND("tms set", cmdTmsSet),
	SHELL_COMMAND("tdi set", cmdTdiSet),
	SHELL_COMMAND("tdo set", cmdTdoSet),
	SHELL_COMMAND("trst set", cmdTrstSet),
	SHELL_COMMAND("loopback check", cmdLoopbackCheck),
	SHELL_COMMAND("ir len set", cmdIrLenSet),
	SHELL_COMMAND("flashsize set", cmdFlashsizeSet),
	SHELL_COMMAND("delay", cmdDelay),
	SHELL_COMMAND("irenum", cmdIrEnum),
	/* NAND */
	SHELL_COMMAND("info nand", cmdInfoNand),
	SHELL_COMMAND("write nand", cmdWriteNand),
	SHELL_COMMAND("dump nand", cmdDumpNand),
	SHELL_COMMAND("config onfi", cmdConfigOnfi),
	SHELL_COMMAND("erase nand", cmdEraseNand),
	/* NOR */
	SHELL_COMMAND("dump nor", cmdDumpNor),
	//SHELL_COMMAND("write nor pattern", cmdNorPattern),
	/* SPI */
	SHELL_COMMAND("spi dump", cmdSpiDump),
	/* UART */
	SHELL_COMMAND("uart scanner", cmdUartScanner),
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
	case CHAR_BACKSPACE:		 //backspace
		if (commandpointer != 0) //delete one char in memory
		{
			uartWriteChar(CHAR_BACKSPACE);
			uartWriteChar(' ');
			uartWriteChar(CHAR_BACKSPACE);
			commandpointer--;
			commandinput[commandpointer] = '\0';
		}
		break;
	case CHAR_ENTER:
		uartWriteString(NL);
		commandinput[commandpointer] = '\0'; // NUL-terminate string
		executeCommand();
		uartWriteString(SHELL_STRING);
		commandpointer = 0;
		commandinput[0] = '\0';
		break;
	default: //default char
		uartWriteChar(character);
		commandinput[commandpointer] = character;
		commandpointer++;
		break;
	}
}

/**
 * @file shell.h
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

#ifndef _SHELL_H
#define _SHELL_H

#define SHELL_LINE_MAX_LENGTH 256

#define CHAR_BACKSPACE 0x08
#define CHAR_ENTER 0x0D

void shellInit(void);
static void shell(char character);

typedef void (shell_command_callback_t)(char *arguments);

typedef struct {
    char *commandPrefix;
    shell_command_callback_t* callback;
} shell_command_t;

#define SHELL_COMMAND(prefix, callback) {(prefix), (&callback)}
#define END_OF_COMMANDS() {(NULL), (NULL)}

#define IS_END_OF_COMMANDS(sc) ((sc)->commandPrefix == NULL)

#endif

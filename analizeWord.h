#ifndef ANALIZE_WORD
#define ANALIZE_WORD

#include "global.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

/*return a number which indicates the command name*/
short getCommandName(char* commandName);

/*return true if given word is instruction*/
bool isInstruction(char* instructionName);

/*return a number which indicates the register name*/
short getRegister(char* registerName);

/*return opcode of a given command name*/
bool isCommandOrInstruction(char* macroName);

/*return funct of a given command name*/
short getOpcode(short commandIndex);

/*return true if given word is command, instruction or register*/
short getFunct(short commandIndex);

#endif 

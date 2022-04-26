#include "analizeWord.h"

static struct CommandTable commandTable[] =
{
	   {"mov", MOV_OP, NO_FUNCT},
	   {"cmp",CMP_OP, NO_FUNCT},
	   {"add",ADD_OP, ADD_FUNCT},
	   {"sub",SUB_OP, SUB_FUNCT},
	   {"lea",LEA_OP, NO_FUNCT},
	   {"clr",CLR_OP, CLR_FUNCT},
	   {"not",NOT_OP, NOT_FUNCT},
	   {"inc",INC_OP, INC_FUNCT},
	   {"dec",DEC_OP, DEC_FUNCT},
	   {"jmp",JMP_OP, JMP_FUNCT},
	   {"bne",BNE_OP, BNE_FUNCT},
	   {"jsr",JSR_OP, JSR_FUNCT},
	   {"red",RED_OP, NO_FUNCT},
	   {"prn",PRN_OP, NO_FUNCT},
	   {"rts",RTS_OP, NO_FUNCT},
	   {"stop",STOP_OP, NO_FUNCT},
	   {NULL, NO_OP, NO_FUNCT}
}; 

static struct instructionLine instructions[] =
{
	{".data"},
	{".string"},
	{".entry"},
	{".extern"}
};
static struct registers registerList[] = {
	{"r0"},
	{"r1"},
	{"r2"},
	{"r3"},
	{"r4"},
	{"r5"},
	{"r6"},
	{"r7"},
	{"r8"},
	{"r9"},
	{"r10"},
	{"r11"},
	{"r12"},
	{"r13"},
	{"r14"},
	{"r15"}
};

/*return a number which indicates the command name*/
short getCommandName(char* commandName) {
	short i;
	for (i = 0; i < 16; i++) {
		if (strcmp(commandName, commandTable[i].command) == 0)
			return i;
	}
	return -1;
}

/*return true if given word is instruction*/
bool isInstruction(char* instructionName) {
	short i;
	for (i = 0; i < 4; i++) {
		if (strcmp(instructionName, instructions[i].name) == 0)
			return true;
	}
	return false;
}

/*return a number which indicates the register name*/
short getRegister(char* registerName) {
	short i;
	for (i = 0; i < 16; i++) {
		if (strcmp(registerName, registerList[i].name) == 0)
			return i;
	}
	return -1;
}

/*return opcode of a given command name*/
short getOpcode(short commandIndex) {
	return commandTable[commandIndex].opcode;
}

/*return funct of a given command name*/
short getFunct(short commandIndex) {
	return commandTable[commandIndex].funct;
}

/*return true if given word is command, instruction or register*/
bool isCommandOrInstruction(char* macroName) {
	bool isCmdOrIns = false;
	if (getCommandName(macroName) != -1 || isInstruction(macroName) == true || getRegister(macroName) != -1)
		isCmdOrIns = true;
	return isCmdOrIns;
}



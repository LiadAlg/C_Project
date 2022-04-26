#ifndef CODE_H
#define CODE_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 80
#define MAX_LABEL_LENGTH 31
#define MAX_RAM_SIZE 8092 
#define MAX_INT_VALUE 32768

typedef enum opcode {
	MOV_OP = 0, CMP_OP = 1, ADD_OP = 2, SUB_OP = 2, LEA_OP = 4,
	CLR_OP = 5, NOT_OP = 5, INC_OP = 5, DEC_OP = 5, JMP_OP = 9, BNE_OP = 9, JSR_OP = 9,
	RED_OP = 12, PRN_OP = 13, RTS_OP = 14, STOP_OP = 15,
	NO_OP = -1
} opcode;


typedef enum funct {
	ADD_FUNCT = 10, SUB_FUNCT = 11,
	CLR_FUNCT = 10, NOT_FUNCT = 11, INC_FUNCT = 12, DEC_FUNCT = 13,
	JMP_FUNCT = 10, BNE_FUNCT = 11, JSR_FUNCT = 12,
	NO_FUNCT = 0
} funct;

typedef enum macroStates {
	MACRO_INVALID = 0, MACRO_VALID = 1, NO_MACRO = 2
}macroStates;

typedef enum Attributes {/*symbol's attributes*/
	CODE,
	DATA,
	ENTRY,
	EXTERNAL
}Attributes;

typedef enum AddressingModes {
	IMMEDIATE = 0,
	DIRECT,
	INDEX,
	REGISTER_DIRECT,
	INVALID_MODE
}AddressingModes;

typedef enum symbolDefinition {
	INVALID_SYMBOL,
	VALID_SYMBOL,
	NO_SYMBOL
} symbolDefinition;

enum ARE {
	E = 1,
	R = 2,
	A = 4
};

typedef struct Macro {
	char* name;
	char* data;
	void* next;
}Macro;

typedef struct MacroList {/*linked list of Macros*/
	int count;
	Macro* head;
}MacroList;

struct instructionLine
{
	char* name;
};

struct registers {
	char* name;
};

struct CommandTable
{
	char* command;
	opcode opcode;
	funct funct;
};

struct CommandName {
	char* commandName;
};

typedef struct Symbol {
	char* name;
	int value;
	int base;
	Attributes attributes;
	void* next;
}Symbol;

typedef struct SymbolTable {/*linked list of symbols*/
	int numOfSymbols;
	Symbol* head;
	Symbol* tail;
}SymbolTable;

typedef struct CodeWord {/*contains values A-E as supossed to be printed to the object file*/
	unsigned int A : 3;
	unsigned int B : 4;
	unsigned int C : 4;
	unsigned int D : 4;
	unsigned int E : 4;
	int ic;
	int labelLine;
	void* next;
}CodeWord;

typedef struct DataImg {/*data image, contains codeWord*/
	int dc;
	CodeWord* head;
	CodeWord* tail;
}DataImg;

typedef struct CodeImg {/*data code, contains codeWord*/
	CodeWord* head;
	CodeWord* tail;
}CodeImg;

typedef struct Extern {
	char* name;
	int Base;
	int Offset;
	void* next;
}Extern;


typedef struct ExternList {/*linked list of externals*/
	int numberOfExtern;
	Extern* head;
	Extern* tail;
}ExternList;


#endif




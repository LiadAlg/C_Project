#ifndef FIRST_PASS
#define FIRST_PASS

#include "global.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

/*creating code image without any label code word. creating data imge , symbol table and external list. return true if succeed*/
bool firstPassProccess(FILE* currentFile, int* ic, int* dc, SymbolTable* symbolTable, CodeImg* codeImg, DataImg* dataImg, ExternList* externList);

/*checks if a given symbol name is valid, and return one of two Symbol's state - invalid or valid. 'symbolDefinition' defined in globals.h */
symbolDefinition isLabelDefinition(char* firstWord, int lineNumber, bool isExtern, SymbolTable* symbolTable);

/*checks if a given symbol name is defined, and return one of two Symbol's state - invalid or valid. 'symbolDefinition' defined in globals.h */
symbolDefinition isDefinedLabel(char* symbolName, bool isExtern, SymbolTable* symbolTable);

/*return true if a given symbol name is semantically valid*/
bool isValidLabel(char* label, int labelLength);

/*add symbol to symbol table*/
void addSymbol(char* symbolName, int symbolValue, int symbolBase, Attributes symbolAttributes, SymbolTable* symbolTable);

/*return true if instruction line was valid and successfully added to data image*/
bool analizeData(char* dataLine, char* stringOrData, int* dc, int lineNumber, int columnNumber, bool success, DataImg* dataImg);

/*add a given number to code or data image*/
void addNumber(int currentNumber, int* ic, int* dc, bool isNegative, bool isCode, int lineNumber, CodeImg* codeImg, DataImg* dataImg);

/*add a given string to data image*/
void addString(char* string, int* dc, short stringLen, DataImg* dataImg);

/*return true if command line was valid and successfully added to code image*/
bool analizeCommand(char* commandLine, char* commandName, int* ic, int lineNumber, int columnNumbervoid, CodeImg* codeImg);

/*return the addressing mode of a given operand. 'AddressingModes' defined in globals.h */
AddressingModes getAddressingMode(char* operand);

/*return pointer to the next word of a given line. also get the word after, depands on 'isFirstOperand'*/
char* getNextWord(char* currentLine, int current, bool numOfOperands, bool isFirstOperand, int lineNumber);

/*add a single code word to code or data image*/
void addCodeWord(short ARE, short sourceReg, short sourceAddress, short targetReg, short targetAddress, int currentLine, int* ic, short index, bool isOpCode, CodeImg* codeImg);

/*return the value of D or E*/
short getDE(short addressingMode, short registerValue, bool isD);

/*returns the hexadecimal value of a give opcode*/
short getopCodeHex(short opcode);

/*return the number as written in a given line*/
int getNum(char* commandLine, int lineNumber);

/*return the offset of a given number*/
int getOffset(int ic);

/*update the symbols that holds the attribute 'DATA', in order to separate data omg from code img*/
void updateSymbols(int* ic, SymbolTable* symbolTable);

/*return the number of digit of a given number*/
int countDigit(int number);

/*checks if a given string is valid and return length of a given string, without apostrophes. return -1 if invalid*/
short stringLength(char* dataLine);


#endif


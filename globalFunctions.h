#ifndef GLOBAL_FUNCTIONS
#define GLOBAL_FUNCTIONS

#include "global.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>


char* strcatChecked(char* str1, char* str2);/*same as strcat, but using malloc and make sure success*/

int skipWhiteSpaces(char* currentFileLine, int firstWordChar);/*skip white spaces of a given line, and a given index to start from */

int skipNonWS(char* currentFileLine, int firstNonWSChar);/*skip non - white spaces of a given line, and a given index to start from */

char* createTempLine(FILE* sourceFile, int lineLength);/*create a line from a given file and the length of the line, using malloc*/

char* createTempWord(char* currentLine, int firstCharInLine, int lastCharInLine);/*create a word from a given file and the length of the line, using malloc */

int getLineLength(FILE* sourceFile);/*return the line Length of a given line from a file*/

int getNumOfLines(FILE* sourceFile);/*return the number of lines in a given file*/

void freeSymbolTable(SymbolTable* symbolTable);/*free all objects of symbolTable*/

void freeDataImg(DataImg* dataImg);/*free all objects of dataImg*/

void freeCodeImg(CodeImg* codeImg, int ic);/*free all objects of codeImg*/

void freeExternList(ExternList* externList);/*free all objects of externList*/

void freeLinkedLists(SymbolTable* symbolTable, CodeImg* codeImg, DataImg* dataImg, ExternList* externList, int ic);/*free all linkedList used in the program*/


#endif

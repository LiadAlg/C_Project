#ifndef SECOND_PASS
#define SECOND_PASS

#include "global.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

/*complete the missing code word in the code image and return true if succeed*/
bool secondPassProccess(FILE* currentFile, int* ic, int* dc, SymbolTable* symbolTable, CodeImg* codeImg, DataImg* dataImg, ExternList* externList);

/*checks if all entry labels are defined and valid. return true if succeed*/
bool validEntries(FILE* currentFile, SymbolTable* symbolTable, int numOfLines);

/*return true if the missing code words were successfuly updated*/
bool isUpdateSucceed(char* labelName, int lineNumber, int currentIC, SymbolTable* symbolTable, ExternList* externList, CodeImg* codeImg);

/*initialize and add two given code words to code image. represent label*/
void intializeWord(CodeWord* codeWord1, CodeWord* codeWord2, int base, int offset);

/*add the two missing code words to the code image*/
void addMissingCode(CodeImg* codeImg, int currentIC, int base, int offset, bool isExtern);

/*return the index of the char '[' in a given string, if '[' does not exist, return 0*/
short getRegDirectIndex(char* label);

/*return the argument of a given line*/
char* getArg(char* currentLine, int* firstChar, int* lastChar, bool isFirstArg);

/*set the file cursor to a given line*/
void moveToLine(FILE* currentFile, int lineNumber, int lastLineChecked);

/*add external to the extern list*/
void addExtern(char* labelName, bool isEntry, int currentIC, int base, int offset, ExternList* externList);



#endif

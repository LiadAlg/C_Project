#ifndef MACRO_DEPLOY
#define MACRO_DEPLOY

#include "global.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>


/*return true if macro deploy was success*/
bool createAmFile(char* filename);

/*returns true if all lines in the file are valid length wise*/
bool validLength(FILE* sourceFile, char* sourceFlileName);

/*return true if a given line is no longer then maximum line length*/
bool excessiveLineLength(int nonWSCharsInLine, int lineNumber);

/*checks if a given line is macro definition and return one of three states - valid macro, invalid macro and no macro.  */
macroStates isValidMacro(char* currentFileLine, int lineNumber, MacroList* macroList);

/*add new macro to macro list*/
void addMacro(char* tempWord, MacroList* macroList);

/*returns true if macro name is defined*/
bool isMacroName(MacroList* macroList, char* macroName);

/*replace a defined macro name to his content as described in the source file*/
void deployMacro(FILE* targetFile, MacroList* macroList, char* macroName);

/*free all objects in macro list*/
void freeMacroList(MacroList* macroList);

#endif

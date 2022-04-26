#ifndef WRITE_FILES
#define WRITE_FILES

#include "global.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

/*creating <fileName>.ob, <fileName>.ent and <fileName>.ext. returns false if one of the above did not created*/
bool writeFileProccess(char* fileName, int ic, int dc, SymbolTable* symbolTable, CodeImg* codeImg, DataImg* dataImg, ExternList* externList);

/*creating <fileName>.ob and return true if succeed*/
bool writeObjectFile(char* fileName, int ic, int dc, CodeImg* codeImg, DataImg* dataImg);

/*creating <fileName>.ent and return true if succeed*/
bool writeEntryFile(char* fileName, SymbolTable* symbolTable);

/*creating <fileName>.txt and return true if succeed*/
bool writeExternFile(char* fileName, ExternList* externList);

#endif

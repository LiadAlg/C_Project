#include "globalFunctions.h"
#include "writeFiles.h"
#include "firstPass.h"

/*creating <fileName>.ob, <fileName>.ent and <fileName>.ext. returns false if one of the above did not created*/
bool writeFileProccess(char* fileName, int ic, int dc, SymbolTable* symbolTable, CodeImg* codeImg, DataImg* dataImg, ExternList* externList) {
	char* fileName_ob, * fileName_ent, * fileName_ext;
	bool isSucceed = true;
	fileName_ob = strcatChecked(fileName, ".ob");
	fileName_ent = strcatChecked(fileName, ".ent");
	fileName_ext = strcatChecked(fileName, ".ext");
	if (writeObjectFile(fileName_ob, ic, dc, codeImg, dataImg) == false)
		isSucceed = false;
	else if (isSucceed == true) {
		if (writeEntryFile(fileName_ent, symbolTable) == false) {
			remove(fileName_ob);/*remove the object file that was previously created*/
			isSucceed = false;
		}
		if (isSucceed == true) {
			if (writeExternFile(fileName_ext, externList) == false) {
				remove(fileName_ob);/*remove the object file that was previously created*/
				remove(fileName_ent);/*remove the entry file that was previously created*/
				isSucceed = false;
			}
		}
	}
	free(fileName_ob);
	free(fileName_ent);
	free(fileName_ext);

	return isSucceed;
}

/*creating <fileName>.ob and return true if succeed*/
bool writeObjectFile(char* fileName, int ic, int dc, CodeImg* codeImg, DataImg* dataImg) {
	int currentIC, totalWords = ic + dc;
	CodeWord* currentCodeWord = codeImg->head;
	FILE* objectFile;
	objectFile = fopen(fileName, "w");
	if (objectFile == NULL) {
		printf("Error creating the file %s\n", fileName);
		free(fileName);
		return false;
	}
	fprintf(objectFile, "\t%d %d\n", ic - 100, dc);
	for (currentIC = 100; currentIC < ic; currentIC++) {
		if (currentIC < 1000)
			fprintf(objectFile, "0");
		fprintf(objectFile, "%d A%x-B%x-C%x-D%x-E%x\n", currentIC, currentCodeWord->A, currentCodeWord->B, currentCodeWord->C, currentCodeWord->D, currentCodeWord->E);
		currentCodeWord = currentCodeWord->next;
	}
	currentCodeWord = dataImg->head;
	for (currentIC = ic; currentIC < totalWords; currentIC++) {
		if (currentIC < 1000)
			fprintf(objectFile, "0");
		fprintf(objectFile, "%d A%x-B%x-C%x-D%x-E%x\n", currentIC, currentCodeWord->A, currentCodeWord->B, currentCodeWord->C, currentCodeWord->D, currentCodeWord->E);
		currentCodeWord = currentCodeWord->next;
	}
	fclose(objectFile);
	return true;
}

/*creating <fileName>.ent and return true if succeed*/
bool writeEntryFile(char* fileName, SymbolTable* symbolTable) {
	int currentSymbolIndex;
	Symbol* currentSymbol = symbolTable->head;
	FILE* entryFile;
	entryFile = fopen(fileName, "w");
	if (entryFile == NULL) {
		printf("Error creating the file %s\n", fileName);
		free(fileName);
		return false;
	}
	for (currentSymbolIndex = 0; currentSymbolIndex < symbolTable->numOfSymbols; currentSymbolIndex++) {
		if (currentSymbol->attributes == ENTRY)
			fprintf(entryFile, "%s,%d,%d\n", currentSymbol->name, currentSymbol->value, currentSymbol->base);
		currentSymbol = currentSymbol->next;
	}
	fclose(entryFile);
	return true;
}

/*creating <fileName>.txt and return true if succeed*/
bool writeExternFile(char* fileName, ExternList* externList) {
	int currentExtIndex;
	Extern* currentExternal = externList->head;
	FILE* externFile;
	externFile = fopen(fileName, "w");
	if (externFile == NULL) {
		printf("Error creating the file %s\n", fileName);
		free(fileName);
		return false;
	}
	for (currentExtIndex = 0; currentExtIndex < externList->numberOfExtern; currentExtIndex++) {
		fprintf(externFile, "%s BASE %d\n%s OFFSET %d\n\n", currentExternal->name, currentExternal->Base, currentExternal->name, currentExternal->Offset);
		currentExternal = currentExternal->next;
	}
	fclose(externFile);
	return true;
}

